#include "../include/CSTransform_to.h"
#include <functional>
#include <vector>

namespace CSUtils
{
  static AbstractPoint enu_to_ccsa_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = enu_to_ccsa(ENUPoint(_point.a, _point.b, _point.c, _point.host));
    return ret.to_abstract();
  }

  static AbstractPoint ccsa_to_enu_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = ccsa_to_enu(CCSAPoint(_point.a, _point.b, _point.c, _point.host));
    return ret.to_abstract();
  }

  static AbstractPoint bcs_to_ccsa_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = bcs_to_ccsa(BCSPoint(_point.a, _point.b, _point.c, _point.host));
    return ret.to_abstract();
  }

  static AbstractPoint ccsa_to_bcs_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = ccsa_to_bcs(CCSAPoint(_point.a, _point.b, _point.c, _point.host));
    return ret.to_abstract();
  }

  static AbstractPoint enu_to_ecef_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = enu_to_ecef(ENUPoint(_point.a, _point.b, _point.c, _point.host));
    return ret.to_abstract();
  }

  static AbstractPoint ecef_to_enu_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = ecef_to_enu(ECEFPoint(_point.a, _point.b, _point.c), _host);
    return ret.to_abstract();
  }

  static AbstractPoint gcs_to_ecef_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = gcs_to_ecef(GCSPoint(_point.a, _point.b, _point.c));
    return ret.to_abstract();
  }

  static AbstractPoint ecef_to_gcs_abstract(AbstractPoint _point, UnitType _host)
  {
    auto ret = ecef_to_gcs(ECEFPoint(_point.a, _point.b, _point.c));
    return ret.to_abstract();
  }

  CSGraph::CSGraph()
  {
    GraphNode* gcs_node = new GraphNode(GCS_CST);
    GraphNode* gccs_node = new GraphNode(ECEF_CST);
    GraphNode* lccs_node = new GraphNode(ENU_CST);
    GraphNode* ccsa_node = new GraphNode(CCSA_CST);
    GraphNode* bcs_node = new GraphNode(BCS_CST);

    gcs_node->node_2 = gccs_node;
    gcs_node->func_2 = gcs_to_ecef_abstract;

    gccs_node->node_1 = gcs_node;
    gccs_node->func_1 = ecef_to_gcs_abstract;
    gccs_node->node_2 = lccs_node;
    gccs_node->func_2 = ecef_to_enu_abstract;

    lccs_node->node_1 = gccs_node;
    lccs_node->func_1 = enu_to_ecef_abstract;
    lccs_node->node_2 = ccsa_node;
    lccs_node->func_2 = enu_to_ccsa_abstract;

    ccsa_node->node_1 = lccs_node;
    ccsa_node->func_1 = ccsa_to_enu_abstract;
    ccsa_node->node_2 = bcs_node;
    ccsa_node->func_2 = ccsa_to_bcs_abstract;

    bcs_node->node_1 = ccsa_node;
    bcs_node->func_1 = bcs_to_ccsa_abstract;

    root = gcs_node;
  }

  GraphNode* CSGraph::find_node(CSType _type, GraphNode* _node) const
  {
    GraphNode* start = root;

    if (start->type == _type)
      return start;

    while (start->node_2 != nullptr)
    {
      start = start->node_2;
      if (start->type == _type)
      {
        return start;
      }
    }
    return nullptr;
  }

  std::vector<std::function<AbstractPoint(AbstractPoint,UnitType)>>
    CSGraph::get_path(CSType _from, CSType _to) const
  {
    std::vector<std::function<AbstractPoint(AbstractPoint,UnitType)>> ret;
    GraphNode* start = find_node(_from, root);

    if (start->type == _to)
      return ret;

    while (start->node_1 != nullptr)
    {
      ret.push_back(start->func_1);
      if (start->node_1->type == _to)
      {
        return ret;
      }
      else
        start = start->node_1;
    }

    ret.clear();
    start = find_node(_from, root);
    while (start->node_2 != nullptr)
    {
      ret.push_back(start->func_2);
      if (start->node_2->type == _to)
      {
        return ret;
      }
      else
        start = start->node_2;
    }

    ret.clear();

    return ret;
  }

  AbstractPoint abstract_transform_to(AbstractPoint _point, CSType _type, UnitType _host)
  {
    std::vector<std::function<AbstractPoint(AbstractPoint,UnitType)>> functors = _graph.get_path(_point.type, _type);
    AbstractPoint ret(_point);
    for (auto f : functors)
      ret = f(ret, _host);

    return ret;
  }
}
