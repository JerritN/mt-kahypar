#pragma once

#include <algorithm>
#include <functional>
#include <unordered_map>
#include <cstddef>
#include <fstream>
#include <istream>
#include <sstream>
#include <string>
#include <string_view>

#include "mt-kahypar/datastructures/metadata/edge_metadata.h"
#include "mt-kahypar/datastructures/hypergraph_common.h"
#include "mt-kahypar/partition/context_enum_classes.h"
#include "mt-kahypar/utils/exception.h"

namespace mt_kahypar::io {

  using EdgeMetadata = ds::EdgeMetadataValue;
  using EdgeMetadataInput = ds::EdgeMetadataInput;

  struct EdgeKey {
    HypernodeID u;
    HypernodeID v;

    bool operator==(const EdgeKey& other) const noexcept {
      return u == other.u && v == other.v;
    }
  };

  inline EdgeKey makeEdgeKey(HypernodeID u, HypernodeID v) {
    return {std::min(u, v), std::max(u, v)};
  }

  struct EdgeKeyHash {
    size_t operator()(const EdgeKey& key) const noexcept {
      const size_t h1 = std::hash<HypernodeID>{}(key.u);
      const size_t h2 = std::hash<HypernodeID>{}(key.v);

      // Combine endpoint hashes using the classic Boost hash_combine formula.
      // https://www.boost.org/doc/libs/1_53_0/doc/html/hash/reference.html
      return h1 ^ (h2 + size_t{0x9e3779b9U} + (h1 << 6) + (h1 >> 2));
    }
  };

  using EdgeLookup = std::unordered_map<EdgeKey, HyperedgeID, EdgeKeyHash>;

  struct NaturalEdgeParser {
    EdgeMetadata parseValue(std::istringstream& stream) const {
      EdgeMetadata value;
      std::string token;

      if (stream >> token) {
        if (token == "0") {
          value.is_natural_edge = false;
        } else if (token == "1") {
          value.is_natural_edge = true;
        } else {
          throw InvalidInputException("Invalid value for natural edge metadata: " + token);
        }
      } else {
        value.is_natural_edge = true; // default to true if no value is provided
      }
      return value;
    }

    EdgeMetadata defaultValue() const {
      return EdgeMetadata{.is_natural_edge = false};
    }
  };

  template<class Graph>
  EdgeLookup buildEdgeLookup(const Graph& graph) {
    EdgeLookup lookup;
    lookup.reserve(graph.maxUniqueID());

    for (const HyperedgeID& he : graph.edges()) {
      const HypernodeID u = graph.edgeSource(he);
      const HypernodeID v = graph.edgeTarget(he);
      const EdgeKey key = makeEdgeKey(u, v);
      const HyperedgeID unique_edge_id = graph.uniqueEdgeID(he);

      const auto [it, inserted] = lookup.emplace(key, unique_edge_id);

      if (!inserted && it->second != unique_edge_id) {
       throw InvalidInputException("Endpoint-based metadata cannot distinguish parallel edges.");
      }
    }
    return lookup;
  }

  template<class Parser>
  void parseEdgeMetadata(std::istream& stream,
                            const EdgeLookup& edge_lookup,
                            EdgeMetadataInput& edge_metadata,
                            Parser parser) {
    std::fill(edge_metadata.values.begin(), edge_metadata.values.end(), parser.defaultValue());

    std::string line;
    while (std::getline(stream, line)) {
      std::istringstream iss(line);
      HypernodeID u, v;
      if (!(iss >> u >> v)) {
        throw InvalidInputException("Invalid edge metadata line: " + line);
      }
      if (u == 0 || v == 0) {
        throw InvalidInputException("Edge metadata line contains invalid node ID (must be >= 1): " + line);
      }

      // Convert to 0-based indexing
      u--; 
      v--;

      EdgeMetadata value = parser.parseValue(iss);

      iss >> std::ws; // consume any whitespace
      if (!iss.eof()) {
        throw InvalidInputException("Unexpected content in edge metadata line: " + line);
      }

      const EdgeKey key = makeEdgeKey(u, v);
      auto it = edge_lookup.find(key);
      if (it == edge_lookup.end()) {
        throw InvalidInputException("Edge (" + std::to_string(u + 1) + ", " + std::to_string(v + 1) + ") not found in graph");
      }
      const HyperedgeID unique_edge_id = it->second;

      edge_metadata.values[unique_edge_id] = value;
    }

    if (stream.bad()) {
      throw InvalidInputException("Error reading edge metadata file");
    }
  }

  template<class Graph>
  EdgeMetadataInput readEdgeMetadataForGraph(const Graph& graph,
                                                const std::string& filename,
                                                EdgeMetadataType type) {
    ASSERT(type != EdgeMetadataType::NONE, "Edge metadata type must not be NONE");
    ASSERT(!filename.empty(), "No filename for edge metadata file specified");

    std::ifstream file(filename);

    if (!file) {
      throw InvalidInputException(std::string("Could not open metadata file: ") + filename);
    }

    auto edge_lookup = buildEdgeLookup(graph);

    EdgeMetadataInput edge_metadata;
    edge_metadata.type = type;
    edge_metadata.values.resize(graph.maxUniqueID());

    switch (type) {
      case EdgeMetadataType::natural_edge:
        parseEdgeMetadata(file, edge_lookup, edge_metadata, NaturalEdgeParser{});
        break;
      default:
        throw InvalidInputException("Unsupported edge metadata type.");
    }

    return edge_metadata;
  }

} // namespace mt_kahypar::io