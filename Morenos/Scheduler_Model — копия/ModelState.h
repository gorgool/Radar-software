#pragma once

#include <cinttypes>
#include <vector>
#include "Query.h"
#include "ControlCommand.h"
#include "Timeline.h"

/*
  Структура описывающая состояние элементов модели Планировщика
*/

struct ModelState
{
  // Модельное время (Время привязки состояния модели)
  std::uint64_t time;

  // Признак прохождения валидации на момент времени привязки
  bool valid_state;

  // Список активных заявок на момент времени привязки
  std::vector<Query> active_queries;

  // Список исполненных заявок на момент времени привязки
  std::vector<Query> processed_queries;

  // Список запланированных команд на момент времени привязки
  std::vector<ControlCommand> planned_commands;

  // Список исполненных команд на момент времени привязки
  std::vector<ControlCommand> execed_commands;

  // Состояние КВД УЦП до попытки размещения команд на момент времени привязки
  Timeline dcu_timeline_before;

  // Состояние КВД УЦП после попытки размещения команд на момент времени привязки
  Timeline dcu_timeline_after;

  // Состояние КВД АУ передатчика до попытки размещения команд на момент времени привязки
  Timeline au_tr_timeline_before;

  // Состояние КВД АУ передатчика после попытки размещения команд на момент времени привязки
  Timeline au_tr_timeline_after;

  // Состояние КВД АУ приемника до попытки размещения команд на момент времени привязки
  Timeline au_rs_timeline_before;

  // Состояние КВД АУ передатчика после попытки размещения команд на момент времени привязки
  Timeline au_rs_timeline_after;
}; 
