#pragma once

#include "mt-kahypar/datastructures/metadata/edge_metadata.h"
#include "mt-kahypar/datastructures/static_graph.h"
#include "mt-kahypar/partition/context_enum_classes.h"

namespace mt_kahypar::io {

ds::EdgeMetadataStorage readEdgeMetadataForGraph(
    const ds::StaticGraph& graph,
    const std::string& filename,
    EdgeMetadataType type);

} // namespace mt_kahypar::io