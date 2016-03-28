#pragma once

#include "Target.h"
#include "Errors.h"
#include <unordered_map>

namespace TargetEnvironment
{
  class TargetTable
  {
  public:

    // Target table type.
    typedef std::unordered_map<std::uint32_t, Target> TableType;

    TargetTable();
    ~TargetTable();

    // Add target to table.
    void add_target(const Target&);

    // Gat target descriptor by id. Return OK if found, SystemError otherwise.
    ErrorCode get_target(Target& target, const std::uint32_t id);

    // Size of the table
    size_t size() const;

    // Clear target table
    void clear();

    // Begin\End itertators
    TableType::iterator begin();
    TableType::iterator end();

  private:

    // Target table
    TableType _tbl;
  };
}

