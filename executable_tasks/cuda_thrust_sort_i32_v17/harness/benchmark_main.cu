#include "interface.h"
#include <cuda_runtime.h>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include <vector>
namespace {
#define CUDA_CHECK(expr) do { cudaError_t err__ = (expr); if (err__ != cudaSuccess) { std::cerr << "{\"ok\":false,\"error\":\"" << cudaGetErrorString(err__) << "\"}\n"; return 1; } } while (0)
constexpr int kN = 32768;
constexpr int kIters = 5;
}  // namespace
int main() {
  std::vector<int> h_in(kN), h_expected;
  uint32_t state = 1013904223u;
  for (int i = 0; i < kN; ++i) { state = state * 1664525u + 1013904223u; h_in[i] = static_cast<int>(state & ((1u << 20) - 1u)); }
  h_expected = h_in;
  std::sort(h_expected.begin(), h_expected.end());
  int *d_in = nullptr, *d_out = nullptr; CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(int))); CUDA_CHECK(cudaMalloc(&d_out, h_in.size() * sizeof(int))); CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(int), cudaMemcpyHostToDevice));
  sort_vec_i32(d_in, d_out, kN, 1); CUDA_CHECK(cudaDeviceSynchronize());
  cudaEvent_t start, stop; CUDA_CHECK(cudaEventCreate(&start)); CUDA_CHECK(cudaEventCreate(&stop)); CUDA_CHECK(cudaEventRecord(start)); sort_vec_i32(d_in, d_out, kN, kIters); CUDA_CHECK(cudaEventRecord(stop)); CUDA_CHECK(cudaEventSynchronize(stop));
  float elapsed_ms = 0.0f; CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));
  std::vector<int> h_out(kN); CUDA_CHECK(cudaMemcpy(h_out.data(), d_out, h_out.size() * sizeof(int), cudaMemcpyDeviceToHost));
  const bool ok = h_out == h_expected; const long long elapsed_ns = static_cast<long long>(elapsed_ms * 1.0e6);
  std::cout << std::boolalpha << "{\"ok\":" << ok << ",\"elapsed_ns\":" << elapsed_ns << "}\n";
  cudaFree(d_in); cudaFree(d_out); cudaEventDestroy(start); cudaEventDestroy(stop); return ok ? 0 : 2;
}
