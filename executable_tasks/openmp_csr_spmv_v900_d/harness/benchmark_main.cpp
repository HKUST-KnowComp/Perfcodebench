#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct CsrInput {
  std::vector<int> row_ptr;
  std::vector<int> col_idx;
  std::vector<int32_t> values;
  std::vector<int32_t> dense;
};

CsrInput make_input(int rows, int cols, int degree) {
  CsrInput out;
  out.row_ptr.resize(static_cast<std::size_t>(rows) + 1);
  out.col_idx.resize(static_cast<std::size_t>(rows) * static_cast<std::size_t>(degree));
  out.values.resize(static_cast<std::size_t>(rows) * static_cast<std::size_t>(degree));
  out.dense.resize(static_cast<std::size_t>(cols));
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (int i = 0; i < cols; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.dense[static_cast<std::size_t>(i)] = static_cast<int32_t>((state % 31ULL) - 15);
  }
  for (int row = 0; row < rows; ++row) {
    out.row_ptr[static_cast<std::size_t>(row)] = row * degree;
    std::vector<int> cols_for_row;
    cols_for_row.reserve(static_cast<std::size_t>(degree));
    for (int j = 0; j < degree; ++j) {
      state = state * 2862933555777941757ULL + 3037000493ULL;
      cols_for_row.push_back(static_cast<int>((state + static_cast<uint64_t>(j) * 17ULL) % static_cast<uint64_t>(cols)));
    }
    std::sort(cols_for_row.begin(), cols_for_row.end());
    for (int j = 0; j < degree; ++j) {
      const std::size_t idx = static_cast<std::size_t>(row) * static_cast<std::size_t>(degree) + static_cast<std::size_t>(j);
      out.col_idx[idx] = cols_for_row[static_cast<std::size_t>(j)];
      state = state * 2862933555777941757ULL + 3037000493ULL;
      out.values[idx] = static_cast<int32_t>((state % 15ULL) - 7);
    }
  }
  out.row_ptr.back() = rows * degree;
  return out;
}

uint64_t checksum_i64(const std::vector<int64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (int64_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(
    const std::vector<int>& row_ptr,
    const std::vector<int>& col_idx,
    const std::vector<int32_t>& values,
    const std::vector<int32_t>& dense,
    int rows) {
  std::vector<int64_t> out(static_cast<std::size_t>(rows), 0);
  for (int row = 0; row < rows; ++row) {
    int64_t sum = 0;
    for (int idx = row_ptr[static_cast<std::size_t>(row)]; idx < row_ptr[static_cast<std::size_t>(row + 1)]; ++idx) {
      sum += static_cast<int64_t>(values[static_cast<std::size_t>(idx)]) *
             static_cast<int64_t>(dense[static_cast<std::size_t>(col_idx[static_cast<std::size_t>(idx)])]);
    }
    out[static_cast<std::size_t>(row)] = sum;
  }
  return checksum_i64(out);
}

}  // namespace

int main() {
  constexpr int kRows = 32768;
  constexpr int kCols = 32768;
  constexpr int kDegree = 32;
  constexpr int kIters = 30;

  const CsrInput input = make_input(kRows, kCols, kDegree);
  const uint64_t expected =
      expected_checksum(input.row_ptr, input.col_idx, input.values, input.dense, kRows);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      csr_spmv_checksum(input.row_ptr, input.col_idx, input.values, input.dense, kRows, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
