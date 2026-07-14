void add_reduce_i32(const int* input, int* output, int n, int iters) {
  if (n <= 0) return;

  // Kernel for large scale reduction using shared memory and warp shuffles
  auto kernel = [] __global__ (const int* __restrict__ input, int* output, int n) {
    extern __shared__ int sdata[];
    int tid = threadIdx.x;
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int sum = 0;

    // Grid-stride loop to handle arbitrary n
    while (i < n) {
      sum += input[i];
      i += blockDim.x * gridDim.x;
    }

    sdata[tid] = sum;
    __syncthreads();

    // Block-level reduction using shuffle
    for (int s = blockDim.x / 2; s > 0; s >>= 1) {
      if (tid < s) {
        sdata[tid] += sdata[tid + s];
      }
      __syncthreads();
    }

    // Single thread per block writes to global via atomic
    if (tid == 0) {
      atomicAdd(output, sdata[0]);
    }
  };

  for (int iter = 0; iter < iters; ++iter) {
    cudaMemset(output, 0, sizeof(int));
    
    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;
    // Cap blocks to prevent excessive overhead if n is massive
    if (blocksPerGrid > 1024) blocksPerGrid = 1024;
    
    kernel<<<blocksPerGrid, threadsPerBlock, threadsPerBlock * sizeof(int)>>>(input, output, n);
  }
}