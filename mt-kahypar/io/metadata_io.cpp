#include "mt-kahypar/io/metadata_io.h"

#include "mt-kahypar/datastructures/metadata/edge_metadata.h"
#include "mt-kahypar/datastructures/static_graph.h"
#include "mt-kahypar/partition/context_enum_classes.h"

namespace mt_kahypar::io {
    ds::EdgeMetadataStorage readEdgeMetadataForGraph(
        const ds::StaticGraph& graph,
        const std::string& filename,
        EdgeMetadataType type) {
        // TODO
        unused(graph);
        unused(filename);
        unused(type);
        throw std::runtime_error("Not implemented yet");
    }
}