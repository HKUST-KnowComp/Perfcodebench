#include "interface.h"

#include <cuda_runtime.h>

#include <cstdint>
#include <cstring>
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

constexpr int kWidth = 1024;
constexpr int kHeight = 1024;
constexpr int kIters = 120;

double checksum(const std::vector<float>& data) {
  double acc = 0.0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    acc += static_cast<double>(data[i]) * static_cast<double>((i % 17) + 1);
  }
  return acc;
}

}  // namespace

int main() {
  std::vector<float> h_in(static_cast<std::size_t>(kWidth) * static_cast<std::size_t>(kHeight));
  std::vector<float> h_expected(h_in.size());
  for (std::size_t i = 0; i < h_in.size(); ++i) {
    h_in[i] = static_cast<float>((i * 19 + 5) % 251) * 0.125f;
  }
  for (int y = 0; y < kHeight; ++y) {
    for (int x = 0; x < kWidth; ++x) {
      h_expected[x * kHeight + y] = h_in[y * kWidth + x];
    }
  }

  float* d_in = nullptr;
  float* d_out = nullptr;
  CUDA_CHECK(cudaMalloc(&d_in, h_in.size() * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_out, h_in.size() * sizeof(float)));
  CUDA_CHECK(cudaMemcpy(d_in, h_in.data(), h_in.size() * sizeof(float), cudaMemcpyHostToDevice));

  transpose_f32(d_in, d_out, kWidth, kHeight, 1);
  CUDA_CHECK(cudaDeviceSynchronize());

  cudaEvent_t start;
  cudaEvent_t stop;
  CUDA_CHECK(cudaEventCreate(&start));
  CUDA_CHECK(cudaEventCreate(&stop));
  CUDA_CHECK(cudaEventRecord(start));
  transpose_f32(d_in, d_out, kWidth, kHeight, kIters);
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));

  float elapsed_ms = 0.0f;
  CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));

  std::vector<float> h_out(h_in.size());
  CUDA_CHECK(cudaMemcpy(h_out.data(), d_out, h_out.size() * sizeof(float), cudaMemcpyDeviceToHost));

  const bool ok = std::memcmp(h_out.data(), h_expected.data(), h_out.size() * sizeof(float)) == 0;
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
