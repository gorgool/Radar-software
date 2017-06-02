#include "CommandProcessor.h"

CommandProcessor::CommandProcessor(const std::string & log_filename) : log(log_filename)
{
  if (log.good() == false)
    throw std::exception();
}

// Добавление массива команд. Команды типа nop игнорируются.

void CommandProcessor::append(const std::vector<ControlCommand>& command_list)
{
  for (const auto& command : command_list)
  {
    if (command.type != CommandType::ct_nop)
      command_queue.push(command);
  }
}

// Выполнение команд на момент времени в диапазоне от time до (time + 1000000) нс

void CommandProcessor::run(const std::uint64_t time)
{
  execed_commands.clear();

  if (command_queue.empty())
    return;

  while (true)
  {
    if (command_queue.empty())
      break;

    auto ev = command_queue.top();
    if (ev.execution_time < time +  1e6)
    {
      execed_commands.push_back(ev);
      // Выполнение команды
      log << ev.execution_time << " [" << ev.query_id << "] : ";
      switch (ev.type)
      {
      case CommandType::ct_receive_command:
        log << "Receive.\n";
        break;
      case CommandType::ct_transmit_command:
        log << "Transmit.\n";
        break;
      case CommandType::ct_tr_rephase_command:
        log << "Transmit Rephase.\n";
        break;
      case CommandType::ct_rs_rephase_command:
        log << "Receive Rephase.\n";
        break;
      case CommandType::ct_protect_command:
        log << "Turn off protection.\n";
        break;
      case CommandType::ct_tech_command:
        log << "Tech.\n";
        break;
      default:
        log << "ERROR: unknown command.\n";
        break;
      }
      // Удаление команды
      command_queue.pop();
    }
    else
    {
      break;
    }
  }
}

// Провверка времени выполнения комманд насоответствование требованиям протокола обмена

bool CommandProcessor::validate()
{
  // Примечание: В текущей версии алгоритма формирования комманд управления команда перефазировки АУ
  // должна быть перед каждой командой УЦП

  // TODO: Переписать так что:
  // 1) Делаем копию залпнированных команд (в массив, порядок не важен)
  // 2) Выполняем проверку по условиям через условный поиск (совпадение по query_id)
  // 3) Проверку на переодичность и неперекрытие комманд

  std::vector<ControlCommand> commands(execed_commands.begin(), execed_commands.end());
  std::vector<ControlCommand> current_commands;
  {
    auto command_copy = command_queue;
    while (command_copy.empty() != true)
    {
      current_commands.push_back(command_copy.top());
      commands.push_back(command_copy.top());
      command_copy.pop();
    }
  }
  
  // Проверка размещения комманд перефазировки АУ передатчика и приемника
  // Требование: команды должны следовать с периодом не менее 500 мкс

  {
    ControlCommand prev_tr_rephase;
    ControlCommand prev_rs_rephase;
    for (const auto& c : current_commands)
    {
      if (c.type == CommandType::ct_tr_rephase_command)
      {
        if (prev_tr_rephase.type == CommandType::ct_nop)
          prev_tr_rephase = c;
        else
        {
          // Проверка условия
          if (c.execution_time - prev_tr_rephase.execution_time < settings::au_command_delay)
          {
            std::cerr << "ERROR: Transmit rephase command is not valid.\n";
            return false;
          }
          else
          {
            prev_tr_rephase = c;
          }
        }
      }
      if (c.type == CommandType::ct_rs_rephase_command)
      {
        if (prev_rs_rephase.type == CommandType::ct_nop)
          prev_rs_rephase = c;
        else
        {
          // Проверка условия
          if (c.execution_time - prev_rs_rephase.execution_time < settings::au_command_delay)
          {
            std::cerr << "ERROR: Receive rephase command is not valid.\n";
            return false;
          }
          else
          {
            prev_rs_rephase = c;
          }
        }
      }
    }
  }

  // Проверка размещения комманд отключения защиты АУ приемника до начала приема
  // Требование: команда отключения защиты должна быть подана не менее чем за 3 мкс до начала приема

  {
    for (const auto& c : current_commands)
    {
      if (c.type == CommandType::ct_receive_command)
      {
        auto it = std::find_if(commands.begin(), commands.end(), [&](const auto& val) { return val.type == CommandType::ct_protect_command && val.query_id == c.query_id; });
        if (it != commands.end())
        {
          if (c.execution_time - it->execution_time < settings::channel_switch_delay)
          {
            std::cerr << "ERROR: Receive command was executed before protection was turned off.\n";
            return false;
          }
        }
        else
        {
          std::cerr << "ERROR: Receiver protection was not turned off.\n";
          return false;
        }
      }
    }
  }

  // Проверка размещения комманд перефазирования АУ приемника до начала приема и перефазирования АУ излучателя до начала излучения
  // Требование: команда перефазирования АУ приемника (излучателя) должна быть подана не менее чем за 270 мкс до начала приема (излучения)

  {
    for (const auto& c : current_commands)
    {
      if (c.type == CommandType::ct_receive_command)
      {
        auto it = std::find_if(commands.begin(), commands.end(), [&](const auto& val) { return val.type == CommandType::ct_rs_rephase_command && val.query_id == c.query_id; });
        if (it != commands.end())
        {
          if (c.execution_time - it->execution_time < settings::phase_delay)
          {
            std::cerr << "ERROR: Receive command was executed before receiver has completed rephase command.\n";
            return false;
          }
        }
        else
        {
          std::cerr << "ERROR: Receiver rephase was not been executed after transmit command.\n";
          return false;
        }
      }

      if (c.type == CommandType::ct_transmit_command)
      {
        auto it = std::find_if(commands.begin(), commands.end(), [&](const auto& val) { return val.type == CommandType::ct_tr_rephase_command && val.query_id == c.query_id; });
        if (it != commands.end())
        {
          if (c.execution_time - it->execution_time < settings::phase_delay)
          {
            std::cerr << "ERROR: Transmit command was executed before transmiter has completed rephase command.\n";
            return false;
          }
        }
        else
        {
          std::cerr << "ERROR: Transmit command was executed before transmiter has completed rephase command.\n";
          return false;
        }
      }
    }
  }

  // Проверка размещения комманд излучения
  // Требование: команда на излучение может быть подана только после истечения времени восстановления после излучения (скважность)

  {
    ControlCommand prev_transmit;
    for (const auto& c : current_commands)
    {
      if (c.type == CommandType::ct_transmit_command)
      {
        if (prev_transmit.type == CommandType::ct_nop)
          prev_transmit = c;
        else
        {
          // Проверка условия
          if (prev_transmit.execution_time + prev_transmit.execution_length >  c.execution_time)
          {
            std::cerr << "ERROR: Transmit command was executed in energy restore state.\n";
            return false;
          }
          else
          {
            prev_transmit = c;
          }
        }
      }
    }
  }

  // Проверка что время пересылки
  // Требование: время пересылки каждого сообщения должно быть раньше чем время выполнения, не менее чем "Предполагаемое время пересылки".

  {
    for (const auto& c : current_commands)
    {
      if (c.type == CommandType::ct_receive_command)
      {

        if (c.execution_time < c.referance_time)
        {
          std::cerr << "ERROR: Execution time is smaller than referance time.\n";
          return false;
        }

        if (c.execution_time - c.referance_time < settings::reserve_time)
        {
          std::cerr
            << "ERROR: Distance between reference time and execution time of receive command is less than "
            << settings::reserve_time << ".\n";
          return false;
        }
      }
      if (c.type == CommandType::ct_transmit_command)
      {
        if (c.execution_time - c.referance_time < settings::reserve_time + settings::blank_delay + settings::frequency_delay)
        {
          std::cerr
            << "ERROR: Distance between reference time and execution of transmit command time is less than "
            << settings::reserve_time + settings::blank_delay + settings::frequency_delay << ".\n";
          return false;
        }
      }
    }
  }

  return true;
}

void CommandProcessor::get_statistics(ModelState & model_state)
{
  auto planned_commands_copy = command_queue;
  while (planned_commands_copy.empty() != true)
  {
    model_state.planned_commands.push_back(planned_commands_copy.top());
    planned_commands_copy.pop();
  }
  
  model_state.execed_commands.resize(execed_commands.size());
  std::copy(execed_commands.begin(), execed_commands.end(), model_state.execed_commands.begin());
}
