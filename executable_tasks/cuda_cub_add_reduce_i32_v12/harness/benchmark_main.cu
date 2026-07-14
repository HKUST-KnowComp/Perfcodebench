#include "interface.h"
#include <cuda_runtime.h>
#include <cstdint>
#include <iostream>
#include <vector>
namespace {
#define CUDA_CHECK(expr) do { cudaError_t err__ = (expr); if (err__ != cudaSuccess) { std::cerr << "{\"ok\":false,\"error\":\"" << cudaGetErrorString(err__) << "\"}\n"; return 1; } } while (0)
constexpr int kN = 1048576;
constexpr int kIters = 100;
}  // namespace
int main() {
  std::vector<int> h_in(kN);
  int64_t expected64 = 0;
  for (int i = 0; i < kN; ++i) { h_in[i] = (i * 7 + 3) & 15; expected64 += h_in[i]; }
  const int expected = static_cast<int>(expected64);
  int *d_in = nullptr, *d_out = nullptr;
  CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(int)));
  CUDA_CHECK(cudaMalloc(&d_out, sizeof(int)));
  CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(int), cudaMemcpyHostToDevice));
  add_reduce_i32(d_in, d_out, kN, 1); CUDA_CHECK(cudaDeviceSynchronize());
  cudaEvent_t start, stop; CUDA_CHECK(cudaEventCreate(&start)); CUDA_CHECK(cudaEventCreate(&stop));
  CUDA_CHECK(cudaEventRecord(start)); add_reduce_i32(d_in, d_out, kN, kIters); CUDA_CHECK(cudaEventRecord(stop)); CUDA_CHECK(cudaEventSynchronize(stop));
  float elapsed_ms = 0.0f; CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));
  int actual = 0; CUDA_CHECK(cudaMemcpy(&actual, d_out, sizeof(int), cudaMemcpyDeviceToHost));
  const bool ok = actual == expected; const long long elapsed_ns = static_cast<long long>(elapsed_ms * 1.0e6);
  std::cout << std::boolalpha << "{\"ok\":" << ok << ",\"elapsed_ns\":" << elapsed_ns << ",\"expected_sum\":" << expected << ",\"actual_sum\":" << actual << "}\n";
  cudaFree(d_in); cudaFree(d_out); cudaEventDestroy(start); cudaEventDestroy(stop); return ok ? 0 : 2;
}
