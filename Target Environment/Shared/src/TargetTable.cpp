#include "../include/TargetTable.h"

namespace TargetEnvironment
{

  TargetTable::TargetTable() {}

  TargetTable::~TargetTable() {}

  void TargetTable::add_target(const TargetDesc &desc)
  {
    _tbl.emplace(desc.target_id, desc);
  }

  TargetTable::TableType& TargetTable::get_table()
  {
    return _tbl;
  }

  void TargetTable::clear()
  {
    _tbl.clear();
  }
}