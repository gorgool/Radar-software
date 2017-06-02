#pragma once

#include <array>
#include <iostream>
#include <functional>
#include "Utility.h"

// ����� ��������� ���
enum TimelineLabel : std::uint8_t
{
  tll_empty,                // ��������
  tll_dcu_receive,          // ����� �� ���
  tll_dcu_transmit,         // ��������� �� ���
  tll_au_rephase,           // ������������� �� ��
  tll_au_command_delay,     // �������� ���������� ������ �� ��
  tll_au_channel_switch,    // ������������ ���������/����������� ������ �� ��
  tll_dcu_energy_restore    // ���������� ������� ����� ��������� �� ���
};

/*
  ���������� �������� ��������� ���������
*/

class Timeline
{
  using pipiline_type = std::array<TimelineLabel, settings::timeline_depth>;

public:

  // ����� �������� ������� �������� ��� (��������� ����������), ��
  std::uint64_t start_time;

  // ������ ������� �������� ���
  std::uint32_t start_idx;

  // ������ ��������� ��������� ��� ���
  pipiline_type timeline;

  // �������� ����������� ���������� ������� ������������� length �� ������� idx �������� �������� pred.
  // ���������� false � ������ ������� � true � ������ ������
  bool check_index(const std::uint32_t length, const std::uint32_t idx, std::function<bool(const pipiline_type::value_type&)> pred);

  // ��������� ������� �� ��� ������� � �������� start_idx �� stop_idx ������ label
  void label_sector(const std::uint32_t start_idx, const std::uint32_t stop_idx, const TimelineLabel label);

  // ���������� ������� �������� ���������. ���������� ������ �� �������� �������� �� ������� idx.
  // ������: ������ 0 ���������� ����� ������ �� ������� ��������� �������.
  TimelineLabel& get_value_at(const std::size_t idx);

  // ���������� ������ �������� ��� ������� time (��) ������������ ������ ���, �.� ����� 0 ������ ������ ������ 0.
  // ���������� -1 � ������ ���� ������ ������� �� ���������� � ������������� �����, ������ �������, � ������ ������.
  std::int32_t get_idx_for(const std::uint32_t time);

  // �������� �������� ������ �� ������� time � ����������� ����� �������.
  // ���������� 
  void move_timeline(std::uint64_t time);

  // ������ ������������ ��������� ���, ��� ��������� ��������� ��������� (tll_empty) �� ���� ���������.
  double occupation();

  Timeline() : start_time(0), start_idx(0) { timeline.assign(TimelineLabel::tll_empty); }

};