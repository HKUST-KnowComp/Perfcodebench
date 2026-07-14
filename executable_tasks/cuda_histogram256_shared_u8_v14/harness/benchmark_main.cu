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

constexpr int kN = 8388608;
constexpr int kIters = 40;

uint64_t checksum(const std::vector<unsigned int>& data) {
  uint64_t acc = 0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    acc += static_cast<uint64_t>(data[i]) * static_cast<uint64_t>(i + 1);
  }
  return acc;
}

}  // namespace

int main() {
  std::vector<unsigned char> h_in(kN);
  std::vector<unsigned int> h_expected(256, 0);
  for (int i = 0; i < kN; ++i) {
    unsigned char value = static_cast<unsigned char>((i * 13 + (i >> 3) + 17) & 0xFF);
    h_in[i] = value;
    ++h_expected[value];
  }

  unsigned char* d_in = nullptr;
  unsigned int* d_out = nullptr;
  CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(unsigned char)));
  CUDA_CHECK(cudaMalloc(&d_out, 256 * sizeof(unsigned int)));
  CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(unsigned char), cudaMemcpyHostToDevice));

  histogram256_u8(d_in, d_out, kN, 1);
  CUDA_CHECK(cudaDeviceSynchronize());

  cudaEvent_t start;
  cudaEvent_t stop;
  CUDA_CHECK(cudaEventCreate(&start));
  CUDA_CHECK(cudaEventCreate(&stop));
  CUDA_CHECK(cudaEventRecord(start));
  histogram256_u8(d_in, d_out, kN, kIters);
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));

  float elapsed_ms = 0.0f;
  CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));

  std::vector<unsigned int> h_out(256);
  CUDA_CHECK(cudaMemcpy(h_out.data(), d_out, 256 * sizeof(unsigned int), cudaMemcpyDeviceToHost));

  bool ok = true;
  for (int i = 0; i < 256; ++i) {
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
