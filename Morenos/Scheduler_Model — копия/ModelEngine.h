#pragma once

#include "EventQueue.h"
#include "Scheduler.h"
#include "CommandProcessor.h"
#include "ModelState.h"
#include <cinttypes>
#include <chrono>
#include <iostream>

class ModelEngine
{
  // КВД
  Scheduler shed;

  // Командный процессор
  CommandProcessor command_processor;

  // Очередь событий моделирования
  EventQueue ev_queue;

  // Текущее время моделирования, мс
  std::uint64_t model_time = 0;

  // Интервал планирования, нс
  std::uint64_t time_step = 1000000;

  // Флаг выполнения процесса моделирования
  bool running = false;

public:

  // Конструктор. Заполняет очередь событий моделирования.
  ModelEngine() : command_processor("log.txt")
  {
    
    // Цель тип №1 - дальность 2000 км, ЭПР = 15 кв.м => пачка 4 импульса по 240 мкс
    // Цель тип №2 - дальность 1000 км, ЭПР = 50 кв.м => 15 мкс
    // Цель тип №3 - дальность 2100 км, ЭПР = 50 кв.м => 240 мкс
    //

    // 0. Выставляем поиск и техконтроль
    ev_queue.add_event(Event(0, [](Scheduler& shed)
    {
      // Первоначальная заявка на поиск всех 3 типов
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 1000, settings::search_query_speed, settings::search_query_threshold, 0, 2.0e6, 15.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 2000, settings::search_query_speed, settings::search_query_threshold, 0, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 3000, settings::search_query_speed, settings::search_query_threshold, 0, 2.1e6, 50.0, true));

      // Первоначальная заявка на тех контроль
      shed.queries.push_back(std::make_shared<Query>(QueryType::tech_control, 100, settings::tech_control_speed, settings::tech_control_threshold, 0, 0.0, 0.0, true));

      return true;
    }));

    // 1. Имитируем обнаружение ОК Тип 1. Дообнаружение (10 такт), Захват(11 такт) и Сопровождение(12 такт) 1 цели
    ev_queue.add_event(Event(10000000, [](Scheduler& shed) 
    { 
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 1001, settings::confirm_query_speed, settings::confirm_query_threashold, 10000000, 2.0e6, 15.0, true));
      return true; 
    }));

    // 2. Имитируем обнаружение 3 ОК (тип №2 и тип №3) в одном угловом направлениии (100 такт)
    ev_queue.add_event(Event(100000000, [](Scheduler& shed)
    {
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 2001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 3001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 4001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 2.1e6, 50.0, true));
      return true;
    }));

    // 3. Снятие с сопровождения всех заявок (1000 такт)
    ev_queue.add_event(Event(1000000000, [](Scheduler& shed)
    {
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 1003, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 2003, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 3003, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 4003, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));

      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 1000, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 2000, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::drop, 3000, settings::tracking_query_speed, settings::tracking_query_threashold, 1000000000, 0.0, 0.0, true));
      
      return true;
    }));

    // 4. Останавливаем моделирование
    ev_queue.add_event(Event(15000000000, [&](Scheduler& shed)
    {
      this->running = false;
      std::cerr << "Model stopped.\n";
      return true;
    }));
  }

  // Запуск цикла моделирования
  void run(const std::uint32_t times = 1)
  {
    running = true;

    while (running)
    {
      // Получение списка событий на текущее время моделирования
      auto events = ev_queue.get_events(model_time);
      
      if (!events.empty())
      {
        // Исполнение событий
        for (auto& ev : events)
        {
          auto ret = ev.exec(shed);
          std::cout << "Event triggered. Model time : " << model_time << "\n";
          if (!ret)
            std::cerr << "Event execution error.\n";
        }
      }

      //if (model_time == 2000000000)
      //  std::cerr << "";

      // Запуск планировщика
      auto command_list = shed.run(model_time);

      // Добавление всех спланированных на текущем такте комманд
      command_processor.append(command_list);

      ModelState ret;

      ret.time = model_time;
      ret.valid_state = command_processor.validate();

      // Выполнение кмманд управления для текущего такта
      command_processor.run(model_time);

      command_processor.get_statistics(ret);
      shed.get_statistics(ret);

      // Увеличение текущего времени моделирования
      model_time += time_step;
    }
  }
};
