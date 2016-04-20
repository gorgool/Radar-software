#include "TargetTable.h"

namespace TargetEnvironment
{

  TargetTable::TargetTable() {}

  TargetTable::~TargetTable() {}

  /**
  * @fn  void TargetTable::add_target(const Target& t);
  *
  * @brief Adds a target.
  *
  * @author  Gorgool
  * @date  20.04.2016
  *
  * @param t The target.
  */

  void TargetTable::add_target(const Target &desc)
  {
    _tbl.emplace(desc.target_id, desc);
  }

  /**
  * @fn  ErrorCode TargetTable::get_target(Target& target, const std::uint32_t id);
  *
  * @brief Gat target descriptor by id.
  *
  * @author  Gorgool
  * @date  20.04.2016
  *
  * @param [out]  target     The Target.
  * @param id                The identifier.
  *
  * @return  OK if found, SystemError otherwise.
  */

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

  /**
  * @fn  size_t TargetTable::size() const;
  *
  * @brief Gets the size of the table.
  *
  * @author  Gorgool
  * @date  20.04.2016
  *
  * @return  A size_t.
  */

  size_t TargetTable::size() const
  {
    return _tbl.size();
  }

  /**
  * @fn  void TargetTable::clear();
  *
  * @brief Clear target table.
  *
  * @author  Gorgool
  * @date  20.04.2016
  */

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