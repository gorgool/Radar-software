#include "Timeline.h"
#include <vector>

bool Timeline::check_index(const std::uint32_t length, const std::uint32_t idx, std::function<bool(const pipiline_type::value_type&)> pred)
{
  if (length > settings::timeline_depth)
  {
    std::cerr << "ERROR: Length is larger than timeline depth.\n";
    return false;
  }

  if (idx > settings::timeline_depth)
  {
    std::cerr << "ERROR: Index is larger than timeline depth.\n";
    return false;
  }

  for (std::uint32_t offset = 0; offset < length; ++offset)
  {
    // Выход за пределы КВД
    if (idx + offset >= settings::timeline_depth)
      return false;

    auto timeline_value = get_value_at(idx + offset);

    // Проверка КВД на метки свободно (free) или время восстановления после излучения (energy_restore)
    if (pred(timeline_value))
      continue;
    else
      return false;
  }

  return true;
}

void Timeline::label_sector(const std::uint32_t start_idx, const std::uint32_t stop_idx, const TimelineLabel label)
{
  if (start_idx > stop_idx)
  {
    std::cerr << "ERROR: Start index is larger than stop index in label function.\n";
    return;
  }

  for (std::size_t idx = start_idx; idx <= stop_idx; ++idx)
  {
    get_value_at(idx) = label;
  }
}

std::int32_t Timeline::get_idx_for(const std::uint32_t time)
{
  if (time > settings::timeline_depth * settings::time_chunk_length)
  {
    std::cerr << "ERROR: Time is out of range.\n";
    return -1;
  }

  auto ret = time / settings::time_chunk_length;

  // Проверка попадания на границу дискрета
  if (time % settings::time_chunk_length == 0)
    // Если попали, то считает как следующий дискрет
    return ret + 1;
  else
    return ret;

}

void Timeline::move_timeline(std::uint64_t time)
{
  if (time < start_time)
  {
    std::cerr << "ERROR: can't move timeline backward.\n";
    return;
  }

  // Смещение по времени
  std::uint32_t time_offset = static_cast<std::uint32_t>(time - start_time);

  // Обновляем время привязки КВД, с учетов выравнивания по границе дискретов КВД
  start_time = multiples_of(time, static_cast<std::uint64_t>(settings::time_chunk_length));

  //if (start_time > settings::time_chunk_length)
  //  start_time -= settings::time_chunk_length;

  // Очищаем крайний "левый" (ближайщий по времени) участок конвейера
  for (std::size_t offset = 0; offset < time_offset / settings::time_chunk_length; ++offset)
    get_value_at(offset) = TimelineLabel::tll_empty;

  // Смещаем индекс первого дискрета КВД
  start_idx = (start_idx + time_offset / settings::time_chunk_length) % settings::timeline_depth;
}

double Timeline::occupation()
{
  std::uint32_t empty_counter = 0;
  for (const auto& chunk : timeline)
  {
    if (chunk == TimelineLabel::tll_empty)
      empty_counter++;
  }

  return static_cast<double>(empty_counter) / settings::timeline_depth;
}

TimelineLabel& Timeline::get_value_at(const std::size_t idx)
{
  return timeline[(start_idx + idx) % settings::timeline_depth];
}
