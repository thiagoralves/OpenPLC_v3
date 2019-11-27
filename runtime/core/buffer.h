// Copyright 2019 Smarter Grid Solutions
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.

#ifndef RUNTIME_CORE_BUFFER_H_
#define RUNTIME_CORE_BUFFER_H_

#include <array>
#include <cstring>

#include "glue.h"

/** \addtogroup openplc_runtime
 *  @{
 */

namespace oplc
{

template<std::size_t N>
struct PackedBoolMappedValues
{
    std::array<std::uint8_t, N> cached_values;
    std::array<GlueBoolGroup*, N> located_values;
    std::size_t max_index;

    PackedBoolMappedValues() : max_index(N)
    {}

    /// @brief Update the cache values with the value from the located
    /// variable that was mapped. This only updated cache items for which
    /// there is a mapping. Unmapped items are not modified.
    void update_cache()
    {
        for (auto i = 0; i < max_index; ++i)
        {
            if (located_values[i])
            {
                auto group = located_values[i];
                std::uint8_t bit_set = 
                    (group->values[0] && *group->values[0] ? 0x01 : 0)
                    | (group->values[1] && *group->values[1] ? 0x02 : 0)
                    | (group->values[2] && *group->values[2] ? 0x03 : 0)
                    | (group->values[3] && *group->values[3] ? 0x04 : 0)
                    | (group->values[4] && *group->values[4] ? 0x10 : 0)
                    | (group->values[5] && *group->values[5] ? 0x20 : 0)
                    | (group->values[6] && *group->values[6] ? 0x40 : 0)
                    | (group->values[7] && *group->values[7] ? 0x80 : 0);
                cached_values[i] = bit_set;
            }
        }
    }

    /// @brief Update the located values with the value from the cache
    /// variable that was mapped. This only updated located items for which
    /// there is a mapping. Unmapped items are not modified.
    void update_located()
    {
        for (auto i = 0; i < max_index; ++i)
        {
            if (located_values[i])
            {
                auto group = located_values[i];
                std::uint8_t cache_item = cached_values[i];
                if (group->values[0])
                {
                    *group->values[0] = cache_item & 0x01 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[1])
                {
                    *group->values[1] = cache_item & 0x02 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[2])
                {
                    *group->values[2] = cache_item & 0x04 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[3])
                {
                    *group->values[3] = cache_item & 0x08 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[4])
                {
                    *group->values[4] = cache_item & 0x10 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[5])
                {
                    *group->values[5] = cache_item & 0x20 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[6])
                {
                    *group->values[6] = cache_item & 0x40 ? IEC_TRUE : IEC_FALSE;
                }
                if (group->values[7])
                {
                    *group->values[7] = cache_item & 0x80 ? IEC_TRUE : IEC_FALSE;
                }
            }
        }
    }

    /// @brief Copy data from the cache to the destination. This copies from
    /// the specified starting index the specified number of items. For
    /// example, if the underlying type of this required 2-bytes per item
    /// and you copy 10 items, then the destination buffer must be at least
    /// 20 bytes in length.
    /// @param dest The destination buffer to copy to.
    /// @param cache_start An offset from the start of the cache containing
    /// the start index in the cache.
    /// @param cache_count The number of bytes in the cache to  copy into the
    /// destination.
    void copy_from_cache(std::uint8_t* dest, std::size_t cache_start, std::size_t cache_count)
    {
        std::memcpy(dest, cached_values.data() + cache_start, cache_count);
    }

    /// @brief Copy data from the source buffer into this cache. This copies
    /// from the beginning fo the source buffer into the cache, starting
    /// at the specified index in the cache.
    void copy_to_cache(std::uint8_t* src, std::size_t src_size, std::size_t cache_start)
    {
        std::memcpy(cached_values.data() + cache_start, src, src_size);
    }
};

/// A set of mapped values that are stored contiguously.
/// Prefer this structure over the simple MappedValue when
/// allocating ranges of located values and where the cached_values
/// buffer will be used directly for IO (to avoid one copy). However,
/// the tradeoff is that updating the cache must iterate over the entire
/// located values.
template <typename T, std::size_t N>
struct ContiguousMappedValues
{
    std::array<T, N> cached_values;
    std::array<T*, N> located_values;
    std::size_t max_index;

    ContiguousMappedValues() : max_index(N)
    {}

    /// @brief Assign a located value into the structure, keeping track
    /// of the maximum assigned index.
    /// @param index The index in the located variables to assign.
    /// @param value Pointer to the located variable that is being assigned.
    void assign(size_t index, T* value)
    {
        located_values[index] = value;
        max_index = std::max(index, max_index);
    }

    /// @brief Update the cache values with the value from the located
    /// variable that was mapped. This only updated cache items for which
    /// there is a mapping. Unmapped items are not modified.
    void update_cache()
    {
        for (auto i = 0; i < max_index; ++i)
        {
            if (located_values[i])
            {
                cached_values[i] = *located_values[i];
            }
        }
    }

    void update_located()
    {
        for (auto i = 0; i < max_index; ++i)
        {
            if (located_values[i])
            {
                *located_values[i] = cached_values[i];
            }
        }
    }

    /// @brief Copy data from the cache to the destination. This copies from
    /// the specified starting index the specified number of items. For
    /// example, if the underlying type of this required 2-bytes per item
    /// and you copy 10 items, then the destination buffer must be at least
    /// 20 bytes in length.
    /// @param dest The destination buffer to copy to.
    /// @param cache_start An offset from the start of the cache containing
    /// the start index in the cache.
    /// @param cache_count The number of items (not bytes) in the cache to
    /// copy into the destination.
    void copy_from_cache(std::uint8_t* dest, std::size_t cache_start, std::size_t cache_count)
    {
        std::memcpy(dest, cached_values.data() + cache_start, cache_count * sizeof(T));
    }

    /// @brief Copy data from the source buffer into this cache. This copies
    /// from the beginning fo the source buffer into the cache, starting
    /// at the specified index in the cache. Note that the index in the cache
    /// is measured in items, not bytes.
    /// If the source size is not an multiple of the item size, then this will
    /// write part of a cache item, which is probably not desired.
    void copy_to_cache(std::uint8_t* src, std::size_t src_size, std::size_t cache_start)
    {
        std::memcpy(cached_values.data() + cache_start * sizeof(T), src, src_size);
    }
};

/// Defines the mapping between a located value
/// and a cached of the located value. This permits
/// an efficient mechanism to exchange data between
/// the cache and located value.
template <typename T>
struct MappedValue
{
    MappedValue() : cached_value(0), value(nullptr) {}
    T cached_value;
    T* value;

    /// Initialize the glue link and the cached value.
    /// @param val The glue variable to initialize from.
    inline void init(T* val)
    {
        this->value = val;
        this->cached_value = *val;
    }

    inline void update_cache()
    {
        if (this->value)
        {
            this->cached_value = *this->value;
        }
    }
};

/// Defines a write that has been submitted via a protocol
/// but may not have been applied to the located variable yet.
/// Normally this would be employed with an array of MappedValue
/// items having the same indices for efficient lookup to the
/// located variable.
template <typename T>
struct PendingValue
{
    PendingValue() : has_pending(false), value(0) {}
    bool has_pending;
    T value;

    /// Set the value and mark it as updated.
    inline void set(T val) {
        this->has_pending = true;
        this->value = val;
    }
};

}  //  namespace oplc

/** @}*/

#endif  // RUNTIME_CORE_MODBUSMASTER_MASTER_H_
