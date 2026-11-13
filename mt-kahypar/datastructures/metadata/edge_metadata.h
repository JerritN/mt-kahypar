#pragma once

#include "mt-kahypar/partition/context_enum_classes.h"

namespace mt_kahypar::ds {

struct EdgeMetadataStorage {
    using Type = EdgeMetadataType;

    union Value {
        bool is_natural_edge;
    };

    EdgeMetadataStorage() : type(Type::NONE) { }
    EdgeMetadataStorage(Type type) : type(type) { }

    Type type;
    Array<Value> values;
};

static_assert(std::is_trivially_copyable_v<EdgeMetadataStorage::Value>);

}  // namespace mt_kahypar::ds