#pragma once

#include <cstdint>
#include <vector>

#define KiB 1024
#define MiB 1024*KiB
#define GiB 1024*MiB

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using s8 = std::int8_t;
using s16 = std::int16_t;
using s32 = std::int32_t;
using s64 = std::int64_t;

using f32 = float;
using f64 = double;

using Byte = u8;
using Buffer = std::vector<Byte>;
