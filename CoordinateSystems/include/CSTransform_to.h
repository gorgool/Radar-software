#pragma once

#include "CSTransform.h"
#include <functional>
#include <vector>

namespace CSUtils
{
  typedef GCSPoint TO_GCS;
  typedef ECEFPoint TO_ECEF;
  typedef ENUPoint TO_ENU;
  typedef BCSPoint TO_BCS;
  typedef CCSAPoint TO_CCSA;

  struct GraphNode
  {
    CSType type;
    GraphNode* node_1;
    GraphNode* node_2;
    std::function<AbstractPoint(AbstractPoint,UnitType)> func_1;
    std::function<AbstractPoint(AbstractPoint,UnitType)> func_2;

    GraphNode(CSType _type, GraphNode* _node_1 = nullptr, GraphNode* _node_2 = nullptr)
      : type(_type), node_1(_node_1), node_2(_node_2) {}
  };

  /*
    Graph to transform any CS in given CS
  */
  struct CSGraph
  {
    GraphNode* root;

    CSGraph();

    GraphNode* find_node(CSType _type, GraphNode* _node) const;

    std::vector<std::function<AbstractPoint(AbstractPoint,UnitType)>>
      get_path(CSType _from, CSType _to) const;
  };

  static CSGraph _graph;

  AbstractPoint abstract_transform_to(AbstractPoint _point, CSType _type, UnitType _host = Tx_UT);

  template< typename T>
  static CSType cs_helper() { return UNKNOWN_CST;}

  template<>
  CSType cs_helper<ENUPoint>() { return ENU_CST; }
  template<>
  CSType cs_helper<CCSAPoint>() { return CCSA_CST; }
  template<>
  CSType cs_helper<BCSPoint>() { return BCS_CST; }
  template<>
  CSType cs_helper<GCSPoint>() { return GCS_CST; }
  template<>
  CSType cs_helper<ECEFPoint>() { return ECEF_CST; }


  // Transform _point in ret_type CS point for given _host (transmitter by default).
  // If transform path not found return _point
  template< typename ret_type, typename in_type >
  ret_type transform_to(in_type _point, UnitType _host = Tx_UT)
  {
    auto ret = abstract_transform_to(_point.to_abstract(), cs_helper<ret_type>(), _host);
    return ret_type(ret);
  }
}
