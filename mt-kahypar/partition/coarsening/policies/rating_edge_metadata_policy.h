#pragma once

#include "mt-kahypar/datastructures/hypergraph_common.h"

#include "kahypar-resources/meta/policy_registry.h"
#include "kahypar-resources/meta/typelist.h"

namespace mt_kahypar {
  using PolicyBase = kahypar::meta::PolicyBase;
  using PolicyRegistry = kahypar::meta::PolicyRegistry<PolicyBase>;

  struct MetadataRating {
    bool include;
    RatingType score;
  };

  struct IgnoreEdgeMetadataPolicy final : public PolicyBase {
    static bool isApplicable(const EdgeMetadataType) {
      return true;
    }

    template<class Graph>
    static MetadataRating evaluate(
        const Graph&, HyperedgeID, RatingType score) {
      return {true, score};
    }
  };

  struct SkipNaturalEdgePolicy final : public PolicyBase {
    static bool isApplicable(const EdgeMetadataType type) {
      return type == EdgeMetadataType::natural_edge;
    }

    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {!natural, score};
    }
  };

  struct NaturalEdgeContributeZeroPolicy final : public PolicyBase {
    static bool isApplicable(const EdgeMetadataType type) {
      return type == EdgeMetadataType::natural_edge;
    }

    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {true, natural ? RatingType{0} : score};
    }
  };

  struct NaturalEdgeContributeNegativePolicy final : public PolicyBase {
    static bool isApplicable(const EdgeMetadataType type) {
      return type == EdgeMetadataType::natural_edge;
    }

    template<class Graph>
    static MetadataRating evaluate(
        const Graph& graph, HyperedgeID edge, RatingType score) {
      const bool natural = graph.edgeMetadata(edge).is_natural_edge;
      return {true, natural ? -score : score};
    }
  };

  using EdgeMetadataRatingPolicies = kahypar::meta::Typelist<
    IgnoreEdgeMetadataPolicy,
    SkipNaturalEdgePolicy,
    NaturalEdgeContributeZeroPolicy,
    NaturalEdgeContributeNegativePolicy
  >;
}