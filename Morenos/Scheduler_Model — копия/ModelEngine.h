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
  // ���
  Scheduler shed;

  // ��������� ���������
  CommandProcessor command_processor;

  // ������� ������� �������������
  EventQueue ev_queue;

  // ������� ����� �������������, ��
  std::uint64_t model_time = 0;

  // �������� ������������, ��
  std::uint64_t time_step = 1000000;

  // ���� ���������� �������� �������������
  bool running = false;

public:

  // �����������. ��������� ������� ������� �������������.
  ModelEngine() : command_processor("log.txt")
  {
    
    // ���� ��� �1 - ��������� 2000 ��, ��� = 15 ��.� => ����� 4 �������� �� 240 ���
    // ���� ��� �2 - ��������� 1000 ��, ��� = 50 ��.� => 15 ���
    // ���� ��� �3 - ��������� 2100 ��, ��� = 50 ��.� => 240 ���
    //

    // 0. ���������� ����� � �����������
    ev_queue.add_event(Event(0, [](Scheduler& shed)
    {
      // �������������� ������ �� ����� ���� 3 �����
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 1000, settings::search_query_speed, settings::search_query_threshold, 0, 2.0e6, 15.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 2000, settings::search_query_speed, settings::search_query_threshold, 0, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::search, 3000, settings::search_query_speed, settings::search_query_threshold, 0, 2.1e6, 50.0, true));

      // �������������� ������ �� ��� ��������
      shed.queries.push_back(std::make_shared<Query>(QueryType::tech_control, 100, settings::tech_control_speed, settings::tech_control_threshold, 0, 0.0, 0.0, true));

      return true;
    }));

    // 1. ��������� ����������� �� ��� 1. ������������� (10 ����), ������(11 ����) � �������������(12 ����) 1 ����
    ev_queue.add_event(Event(10000000, [](Scheduler& shed) 
    { 
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 1001, settings::confirm_query_speed, settings::confirm_query_threashold, 10000000, 2.0e6, 15.0, true));
      return true; 
    }));

    // 2. ��������� ����������� 3 �� (��� �2 � ��� �3) � ����� ������� ������������ (100 ����)
    ev_queue.add_event(Event(100000000, [](Scheduler& shed)
    {
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 2001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 3001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 1.0e6, 50.0, true));
      shed.queries.push_back(std::make_shared<Query>(QueryType::confirm, 4001, settings::confirm_query_speed, settings::confirm_query_threashold, 100000000, 2.1e6, 50.0, true));
      return true;
    }));

    // 3. ������ � ������������� ���� ������ (1000 ����)
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

    // 4. ������������� �������������
    ev_queue.add_event(Event(15000000000, [&](Scheduler& shed)
    {
      this->running = false;
      std::cerr << "Model stopped.\n";
      return true;
    }));
  }

  // ������ ����� �������������
  void run(const std::uint32_t times = 1)
  {
    running = true;

    while (running)
    {
      // ��������� ������ ������� �� ������� ����� �������������
      auto events = ev_queue.get_events(model_time);
      
      if (!events.empty())
      {
        // ���������� �������
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

      // ������ ������������
      auto command_list = shed.run(model_time);

      // ���������� ���� �������������� �� ������� ����� �������
      command_processor.append(command_list);

      ModelState ret;

      ret.time = model_time;
      ret.valid_state = command_processor.validate();

      // ���������� ������ ���������� ��� �������� �����
      command_processor.run(model_time);

      command_processor.get_statistics(ret);
      shed.get_statistics(ret);

      // ���������� �������� ������� �������������
      model_time += time_step;
    }
  }
};
