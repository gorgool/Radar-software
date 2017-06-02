#pragma once
#include <queue>
#include <cinttypes>
#include <fstream>
#include <string>
#include <iostream>

#include "Settings.h"
#include "ModelState.h"
#include "ControlCommand.h"


/*
  Командный процессор. Обеспечивающий храние очереди команд и 
  выполнение команд управления на заданный момент времени. 
*/
class CommandProcessor
{
  // Очередь команд
  std::priority_queue<ControlCommand> command_queue;

  // Массив исполненных в последнем запуске функции run команд
  std::vector<ControlCommand> execed_commands;

  // Файл лога исполнения комманд
  std::ofstream log;
public:

  CommandProcessor(const std::string& log_filename);

  // Добавление массива команд. Команды типа nop игнорируются.
  void append(const std::vector<ControlCommand>& command_list);

  // Выполнение команд на момент времени в диапазоне от time до (time + 1) мс
  void run(const std::uint64_t time);

  // Провверка времени выполнения комманд насоответствование требованиям протокола обмена
  bool validate();

  // Заполняет переданную структуру model_state параметрами запланированных и исполненных в последнем запуске ф-ии run команд
  void get_statistics(ModelState& model_state);
};