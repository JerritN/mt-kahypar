#pragma once

#include "mt-kahypar/datastructures/metadata/edge_metadata.h"

namespace mt_kahypar::ds {

struct NoEdgeMetadataContractionPolicy {
    static constexpr bool enabled = false;
    static constexpr EdgeMetadataStorage::Type type =
        EdgeMetadataStorage::Type::NONE;

    using Value = EdgeMetadataStorage::Value;

    Value contract(const Value&, const Value&) const noexcept {
        return Value{};
    }
};

struct NaturalEdgeMetadataContractionPolicy {
    static constexpr bool enabled = true;
    static constexpr EdgeMetadataStorage::Type type =
        EdgeMetadataStorage::Type::natural_edge;

    using Value = EdgeMetadataStorage::Value;

    Value contract(const Value& lhs, const Value& rhs) const noexcept {
        Value result{};
        result.is_natural_edge = lhs.is_natural_edge || rhs.is_natural_edge;
        return result;
    }
};

}  // namespace mt_kahypar::ds