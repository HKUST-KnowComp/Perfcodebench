#include "interface.h"
#include <cuda_runtime.h>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <vector>
namespace {
#define CUDA_CHECK(expr) do { cudaError_t err__ = (expr); if (err__ != cudaSuccess) { std::cerr << "{\"ok\":false,\"error\":\"" << cudaGetErrorString(err__) << "\"}\n"; return 1; } } while (0)
constexpr int kN = 1 << 20;
constexpr int kIters = 20;
}  // namespace
int main() {
  std::vector<float> h_in(kN); double expected = 0.0; for (int i = 0; i < kN; ++i) { h_in[i] = static_cast<float>((i % 97) * 0.25f); expected += h_in[i]; }
  float* d_in = nullptr; CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(float))); CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(float), cudaMemcpyHostToDevice));
  float actual0 = reduce_f32(d_in, kN, 1); (void)actual0; CUDA_CHECK(cudaDeviceSynchronize());
  cudaEvent_t start, stop; CUDA_CHECK(cudaEventCreate(&start)); CUDA_CHECK(cudaEventCreate(&stop)); CUDA_CHECK(cudaEventRecord(start)); float actual = reduce_f32(d_in, kN, kIters); CUDA_CHECK(cudaEventRecord(stop)); CUDA_CHECK(cudaEventSynchronize(stop));
  float elapsed_ms = 0.0f; CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));
  const bool ok = std::fabs(static_cast<double>(actual) - expected) <= 1e-2 * expected + 1e-2; const long long elapsed_ns = static_cast<long long>(elapsed_ms * 1.0e6);
  std::cout << std::boolalpha << "{\"ok\":" << ok << ",\"elapsed_ns\":" << elapsed_ns << ",\"expected_sum\":" << expected << ",\"actual_sum\":" << actual << "}\n";
  cudaFree(d_in); cudaEventDestroy(start); cudaEventDestroy(stop); return ok ? 0 : 2;
}
