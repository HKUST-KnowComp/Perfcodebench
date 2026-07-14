public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0L;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    // Allocate once and reuse; ensure it starts zeroed
    long[] hist = new long[buckets];

    final int n = keys.length;

    for (int iter = 0; iter < iters; iter++) {
      // Accumulate into histogram (unrolled by 4)
      int i = 0;
      int unrollEnd = n & ~3; // largest multiple of 4 <= n
      for (; i < unrollEnd; i += 4) {
        int k0 = keys[i];
        int k1 = keys[i + 1];
        int k2 = keys[i + 2];
        int k3 = keys[i + 3];
        hist[k0] += vals[i];
        hist[k1] += vals[i + 1];
        hist[k2] += vals[i + 2];
        hist[k3] += vals[i + 3];
      }
      for (; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      // Compute hash over all buckets and clear histogram for next iteration
      h = FNV_OFFSET;
      int b = 0;
      int bunroll = buckets & ~3; // unroll by 4
      for (; b < bunroll; b += 4) {
        long v0 = hist[b];
        long v1 = hist[b + 1];
        long v2 = hist[b + 2];
        long v3 = hist[b + 3];

        h ^= (v0 + 1L);
        h *= FNV_PRIME;
        h ^= (v1 + 1L);
        h *= FNV_PRIME;
        h ^= (v2 + 1L);
        h *= FNV_PRIME;
        h ^= (v3 + 1L);
        h *= FNV_PRIME;

        hist[b] = 0L;
        hist[b + 1] = 0L;
        hist[b + 2] = 0L;
        hist[b + 3] = 0L;
      }
      for (; b < buckets; b++) {
        long v = hist[b];
        h ^= (v + 1L);
        h *= FNV_PRIME;
        hist[b] = 0L;
      }
    }

    return h;
  }
}