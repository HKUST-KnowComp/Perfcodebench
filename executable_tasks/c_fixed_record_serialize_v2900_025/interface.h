#pragma once

#include <stddef.h>
#include <stdint.h>

uint64_t run(const uint32_t* ids, const uint32_t* ts, const uint16_t* levels, const uint16_t* lens, size_t count, int iters);
