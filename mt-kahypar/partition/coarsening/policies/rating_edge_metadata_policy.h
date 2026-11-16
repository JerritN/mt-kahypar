#pragma once

#include "mt-kahypar/datastructures/hypergraph_common.h"

namespace mt_kahypar {
  struct MetadataRating {
    bool include;
    RatingType score;
  };

  struct IgnoreEdgeMetadataPolicy {
    template<class Graph>
    static MetadataRating evaluate(
        const Graph&, HyperedgeID, RatingType score) {
      return {true, score};
    }
  };

  struct SkipNaturalEdgePolicy {
    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {!natural, score};
    }
  };

  struct NaturalEdgeContributeZeroPolicy {
    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {true, natural ? RatingType{0} : score};
    }
  };

  struct NaturalEdgeContributeNegativePolicy {
    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {true, natural ? -score : score};
    }
  };
}