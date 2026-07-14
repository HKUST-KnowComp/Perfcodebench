#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 0x100000001b3ULL;  // 1099511628211ULL
  return hash;
}

}  // namespace

uint64_t csr_offsets_hash(const std::vector<uint32_t>& row_ids, uint32_t rows, int iters) {
  const uint64_t fnv_offset_basis = 0xcbf29ce484222325ULL;  // 14695981039346656037ULL
  const uint64_t fnv_prime        = 0x100000001b3ULL;      // 1099511628211ULL

  const std::size_t rows_plus1 = static_cast<std::size_t>(rows) + 1u;
  std::vector<uint32_t> offsets;
  offsets.reserve(rows_plus1);
  std::vector<uint32_t> counts(rows, 0u);

  for (int iter = 0; iter < iters; ++iter) {
    const std::size_t nnz = row_ids.size();
    if (rows > 0) {
      ::std::size_t first_row = row_ids[0];
      ::std::size_t last_row  = row_ids[static_cast<std::size_t>(nnz - 1u)];
      std::fill(counts.begin() + first_row, counts.begin() + (last_row + 1u), 0u);
    }
    // fast histogram from sorted data
    for (std::size_t i = 0; i < nnz; ++i) {
      ++counts[row_ids[i]];
    }
    // build offsets
    offsets.resize(rows_plus1, 0u);
    std::size_t sum = 0;
    for (std::size_t r = 0; r < rows; ++r) {
      std::size_t tmp = sum;
      sum += counts[r];
      offsets[r + 1] = static_cast<uint32_t>(sum);
    }
    // rolling hash over the offsets array
    uint64_t hash = fnv_offset_basis;
    const uint32_t* ptr = offsets.data();
    const uint32_t* end = ptr + (rows_plus1);
    while (ptr != end) {
      hash = mix(hash, *ptr++);
    }
  }
  return hash;
}