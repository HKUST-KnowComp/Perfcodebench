#include "interface.h"

#include <cuda_runtime.h>

#include <cstdint>
#include <iostream>
#include <vector>

namespace {

#define CUDA_CHECK(expr)                                                                          \
  do {                                                                                            \
    cudaError_t err__ = (expr);                                                                   \
    if (err__ != cudaSuccess) {                                                                   \
      std::cerr << "{\"ok\":false,\"error\":\"" << cudaGetErrorString(err__) << "\"}\n";         \
      return 1;                                                                                   \
    }                                                                                             \
  } while (0)

constexpr int kN = 1048576;
constexpr int kIters = 100;

uint64_t checksum(const std::vector<int>& data) {
  uint64_t acc = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    acc += static_cast<uint64_t>(static_cast<uint32_t>(data[i])) * static_cast<uint64_t>((i % 29) + 1);
  }
  return acc;
}

}  // namespace

int main() {
  std::vector<int> h_in(kN);
  std::vector<int> h_expected(kN);
  int running = 0;
  for (int i = 0; i < kN; ++i) {
    h_in[i] = (i * 5 + 1) & 7;
    h_expected[i] = running;
    running += h_in[i];
  }

  int* d_in = nullptr;
  int* d_out = nullptr;
  CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(int)));
  CUDA_CHECK(cudaMalloc(&d_out, h_in.size() * sizeof(int)));
  CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(int), cudaMemcpyHostToDevice));

  exclusive_scan_i32(d_in, d_out, kN, 1);
  CUDA_CHECK(cudaDeviceSynchronize());

  cudaEvent_t start;
  cudaEvent_t stop;
  CUDA_CHECK(cudaEventCreate(&start));
  CUDA_CHECK(cudaEventCreate(&stop));
  CUDA_CHECK(cudaEventRecord(start));
  exclusive_scan_i32(d_in, d_out, kN, kIters);
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));

  float elapsed_ms = 0.0f;
  CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));

  std::vector<int> h_out(kN);
  CUDA_CHECK(cudaMemcpy(h_out.data(), d_out, h_out.size() * sizeof(int), cudaMemcpyDeviceToHost));

  bool ok = true;
  for (int i = 0; i < kN; ++i) {
    if (h_out[i] != h_expected[i]) {
      ok = false;
      break;
    }
  }
  const long long elapsed_ns = static_cast<long long>(elapsed_ms * 1.0e6);

  std::cout << std::boolalpha;
  std::cout << "{\"ok\":" << ok << ",\"elapsed_ns\":" << elapsed_ns
            << ",\"checksum\":" << checksum(h_out) << "}\n";

  cudaFree(d_in);
  cudaFree(d_out);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  return ok ? 0 : 2;
}
