public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters <= 0) return 0L;

    final int n = keys.length;
    final int[] k = keys;
    final int[] v = vals;
    final long[] hist = new long[buckets]; // zero-initialized once

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      // Accumulate into histogram
      int i = 0;
      int n4 = n - 4;
      for (; i <= n4; i += 4) {
        int k0 = k[i];
        int k1 = k[i + 1];
        int k2 = k[i + 2];
        int k3 = k[i + 3];
        hist[k0] += (long) v[i];
        hist[k1] += (long) v[i + 1];
        hist[k2] += (long) v[i + 2];
        hist[k3] += (long) v[i + 3];
      }
      for (; i < n; i++) {
        hist[k[i]] += (long) v[i];
      }

      // Compute checksum and reset histogram for next iteration
      long hh = FNV64_OFFSET;
      int b = 0;
      int b4 = buckets - 4;
      for (; b <= b4; b += 4) {
        long v0 = hist[b];
        hh ^= (v0 + 1L);
        hh *= FNV64_PRIME;
        hist[b] = 0L;

        long v1 = hist[b + 1];
        hh ^= (v1 + 1L);
        hh *= FNV64_PRIME;
        hist[b + 1] = 0L;

        long v2 = hist[b + 2];
        hh ^= (v2 + 1L);
        hh *= FNV64_PRIME;
        hist[b + 2] = 0L;

        long v3 = hist[b + 3];
        hh ^= (v3 + 1L);
        hh *= FNV64_PRIME;
        hist[b + 3] = 0L;
      }
      for (; b < buckets; b++) {
        long vb = hist[b];
        hh ^= (vb + 1L);
        hh *= FNV64_PRIME;
        hist[b] = 0L;
      }

      h = hh;
    }

    return h;
  }
}
