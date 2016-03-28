#include "TargetTable.h"

namespace TargetEnvironment
{

  TargetTable::TargetTable() {}

  TargetTable::~TargetTable() {}

  void TargetTable::add_target(const Target &desc)
  {
    _tbl.emplace(desc.target_id, desc);
  }

  ErrorCode TargetTable::get_target(Target& target, const std::uint32_t id)
  {
    auto it = _tbl.find(id);

    if (it == end())
      return ErrorCode::SystemError;
    else
    {
      target = it->second;
      return ErrorCode::OK;
    }
  }

  size_t TargetTable::size() const
  {
    return _tbl.size();
  }

  void TargetTable::clear()
  {
    _tbl.clear();
  }

  TargetTable::TableType::iterator TargetTable::begin()
  {
    return _tbl.begin();
  }

  TargetTable::TableType::iterator TargetTable::end()
  {
    return _tbl.end();
  }
}