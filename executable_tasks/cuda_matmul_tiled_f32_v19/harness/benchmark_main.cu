#include "interface.h"

#include <cuda_runtime.h>

#include <algorithm>
#include <cmath>
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

constexpr int kN = 448;
constexpr int kIters = 12;

void make_inputs(std::vector<float>& a, std::vector<float>& b) {
  a.resize(kN * kN);
  b.resize(kN * kN);
  for (int i = 0; i < kN * kN; ++i) {
    a[i] = static_cast<float>((i * 13 + 7) % 97) * 0.03125f;
    b[i] = static_cast<float>((i * 17 + 11) % 89) * 0.02734375f;
  }
}

void cpu_matmul(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& c) {
  c.assign(kN * kN, 0.0f);
  for (int row = 0; row < kN; ++row) {
    for (int k = 0; k < kN; ++k) {
      const float av = a[row * kN + k];
      const int c_off = row * kN;
      const int b_off = k * kN;
      for (int col = 0; col < kN; ++col) {
        c[c_off + col] += av * b[b_off + col];
      }
    }
  }
}

double checksum(const std::vector<float>& data) {
  double acc = 0.0;
  for (std::size_t i = 0; i < data.size(); ++i) {
    acc += static_cast<double>(data[i]) * static_cast<double>((i % 23) + 1);
  }
  return acc;
}

}  // namespace

int main() {
  std::vector<float> h_a;
  std::vector<float> h_b;
  std::vector<float> h_expected;
  make_inputs(h_a, h_b);
  cpu_matmul(h_a, h_b, h_expected);

  float* d_a = nullptr;
  float* d_b = nullptr;
  float* d_c = nullptr;
  CUDA_CHECK(cudaMalloc(&d_a, h_a.size() * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_b, h_b.size() * sizeof(float)));
  CUDA_CHECK(cudaMalloc(&d_c, h_expected.size() * sizeof(float)));
  CUDA_CHECK(cudaMemcpy(d_a, h_a.data(), h_a.size() * sizeof(float), cudaMemcpyHostToDevice));
  CUDA_CHECK(cudaMemcpy(d_b, h_b.data(), h_b.size() * sizeof(float), cudaMemcpyHostToDevice));

  matmul_f32(d_a, d_b, d_c, kN, 1);
  CUDA_CHECK(cudaDeviceSynchronize());

  cudaEvent_t start;
  cudaEvent_t stop;
  CUDA_CHECK(cudaEventCreate(&start));
  CUDA_CHECK(cudaEventCreate(&stop));
  CUDA_CHECK(cudaEventRecord(start));
  matmul_f32(d_a, d_b, d_c, kN, kIters);
  CUDA_CHECK(cudaEventRecord(stop));
  CUDA_CHECK(cudaEventSynchronize(stop));

  float elapsed_ms = 0.0f;
  CUDA_CHECK(cudaEventElapsedTime(&elapsed_ms, start, stop));

  std::vector<float> h_out(h_expected.size());
  CUDA_CHECK(cudaMemcpy(h_out.data(), d_c, h_out.size() * sizeof(float), cudaMemcpyDeviceToHost));

  float max_abs_error = 0.0f;
  for (std::size_t i = 0; i < h_out.size(); ++i) {
    max_abs_error = std::max(max_abs_error, std::fabs(h_out[i] - h_expected[i]));
  }
  const bool ok = max_abs_error <= 1e-2f;
  const long long elapsed_ns = static_cast<long long>(elapsed_ms * 1.0e6);

  std::cout << std::boolalpha;
  std::cout << "{\"ok\":" << ok << ",\"elapsed_ns\":" << elapsed_ns
            << ",\"max_abs_error\":" << max_abs_error << ",\"checksum\":" << checksum(h_out) << "}\n";

  cudaFree(d_a);
  cudaFree(d_b);
  cudaFree(d_c);
  cudaEventDestroy(start);
  cudaEventDestroy(stop);
  return ok ? 0 : 2;
}
