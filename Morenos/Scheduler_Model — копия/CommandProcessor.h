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
  ��������� ���������. �������������� ������ ������� ������ � 
  ���������� ������ ���������� �� �������� ������ �������. 
*/
class CommandProcessor
{
  // ������� ������
  std::priority_queue<ControlCommand> command_queue;

  // ������ ����������� � ��������� ������� ������� run ������
  std::vector<ControlCommand> execed_commands;

  // ���� ���� ���������� �������
  std::ofstream log;
public:

  CommandProcessor(const std::string& log_filename);

  // ���������� ������� ������. ������� ���� nop ������������.
  void append(const std::vector<ControlCommand>& command_list);

  // ���������� ������ �� ������ ������� � ��������� �� time �� (time + 1) ��
  void run(const std::uint64_t time);

  // ��������� ������� ���������� ������� ������������������ ����������� ��������� ������
  bool validate();

  // ��������� ���������� ��������� model_state ����������� ��������������� � ����������� � ��������� ������� �-�� run ������
  void get_statistics(ModelState& model_state);
};