#pragma once

#include <cinttypes>
#include <vector>
#include "Query.h"
#include "ControlCommand.h"
#include "Timeline.h"

/*
  ��������� ����������� ��������� ��������� ������ ������������
*/

struct ModelState
{
  // ��������� ����� (����� �������� ��������� ������)
  std::uint64_t time;

  // ������� ����������� ��������� �� ������ ������� ��������
  bool valid_state;

  // ������ �������� ������ �� ������ ������� ��������
  std::vector<Query> active_queries;

  // ������ ����������� ������ �� ������ ������� ��������
  std::vector<Query> processed_queries;

  // ������ ��������������� ������ �� ������ ������� ��������
  std::vector<ControlCommand> planned_commands;

  // ������ ����������� ������ �� ������ ������� ��������
  std::vector<ControlCommand> execed_commands;

  // ��������� ��� ��� �� ������� ���������� ������ �� ������ ������� ��������
  Timeline dcu_timeline_before;

  // ��������� ��� ��� ����� ������� ���������� ������ �� ������ ������� ��������
  Timeline dcu_timeline_after;

  // ��������� ��� �� ����������� �� ������� ���������� ������ �� ������ ������� ��������
  Timeline au_tr_timeline_before;

  // ��������� ��� �� ����������� ����� ������� ���������� ������ �� ������ ������� ��������
  Timeline au_tr_timeline_after;

  // ��������� ��� �� ��������� �� ������� ���������� ������ �� ������ ������� ��������
  Timeline au_rs_timeline_before;

  // ��������� ��� �� ����������� ����� ������� ���������� ������ �� ������ ������� ��������
  Timeline au_rs_timeline_after;
}; 
