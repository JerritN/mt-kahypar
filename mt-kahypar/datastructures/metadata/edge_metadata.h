#pragma once

#include <type_traits>

#include "mt-kahypar/parallel/stl/scalable_vector.h"
#include "mt-kahypar/partition/context_enum_classes.h"

namespace mt_kahypar::ds {

union EdgeMetadataValue {
    bool is_natural_edge;
};

static_assert(std::is_trivially_copyable_v<EdgeMetadataValue>);

struct EdgeMetadataInput {
  EdgeMetadataType type = EdgeMetadataType::NONE;

  // Indexed by graph unique edge ID.
  vec<EdgeMetadataValue> values;
};

}  // namespace mt_kahypar::ds