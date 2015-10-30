#pragma once

#include "TargetEnvironmentMessages.h"
#include <unordered_map>

namespace TargetEnvironment
{
  class TargetTable
  {
  public:
    TargetTable();
    ~TargetTable();

    // Target table type.
    typedef std::unordered_map<std::size_t, TargetDesc> TableType;

    // Add target to table.
    void add_target(const TargetDesc&);

    // Clear target table
    void clear();

    // Get reference to target table
    TableType& get_table();

  private:
    TableType _tbl;
  };
}

