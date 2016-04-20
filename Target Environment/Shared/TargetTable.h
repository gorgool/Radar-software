#pragma once

#include "Target.h"
#include "Errors.h"
#include <unordered_map>

namespace TargetEnvironment
{
  /**
   * @class TargetTable
   *
   * @brief A target table.
   *
   * @author  Gorgool
   * @date  20.04.2016
   */

  class TargetTable
  {
  public:

    /**
     * @typedef std::unordered_map<std::uint32_t, Target> TableType
     *
     * @brief Defines an alias representing type of the table.
     */

    typedef std::unordered_map<std::uint32_t, Target> TableType;

    TargetTable();
    ~TargetTable();

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

    void add_target(const Target& t);

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

    ErrorCode get_target(Target& target, const std::uint32_t id);

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

    size_t size() const;

    /**
     * @fn  void TargetTable::clear();
     *
     * @brief Clear target table.
     *
     * @author  Gorgool
     * @date  20.04.2016
     */

    void clear();

    // Begin\End itertators
    TableType::iterator begin();
    TableType::iterator end();

  private:

    /** @brief The table. */
    TableType _tbl;
  };
}

