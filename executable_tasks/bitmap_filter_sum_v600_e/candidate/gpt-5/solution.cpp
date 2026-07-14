#include "interface.h"

uint64_t bitmap_filter_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space,
    int iters) {
  (void)iters;

  // Build a bitmap of enabled keys once.
  std::vector<uint8_t> bitmap(key_space, 0);
  const uint32_t* ek = enabled_keys.data();
  const uint32_t* ek_end = ek + enabled_keys.size();
  for (; ek != ek_end; ++ek) {
    // Assuming keys are within [0, key_space) per task contract.
    bitmap[*ek] = 1;
  }

  // Sum values for probes that are enabled.
  const uint32_t* pk = probe_keys.data();
  const uint32_t* pk_end = pk + probe_keys.size();
  const uint32_t* vals = values.data();

  uint64_t sum = 0;
  for (std::size_t i = 0; pk + i != pk_end; ++i) {
    sum += static_cast<uint64_t>(vals[i]) * static_cast<uint64_t>(bitmap[pk[i]]);
  }

  return sum;
}
