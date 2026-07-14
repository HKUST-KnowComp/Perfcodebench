public class Solution {
  private static final long FNV64_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int len = keys.length;
    final long[] hist = new long[buckets];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      // Build histogram with simple unrolling
      int i = 0;
      int n = len & ~3; // process in chunks of 4
      for (; i < n; i += 4) {
        int k0 = keys[i];
        int k1 = keys[i + 1];
        int k2 = keys[i + 2];
        int k3 = keys[i + 3];
        hist[k0] += vals[i];
        hist[k1] += vals[i + 1];
        hist[k2] += vals[i + 2];
        hist[k3] += vals[i + 3];
      }
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }

      // Compute checksum and clear histogram for next iteration
      h = FNV64_OFFSET_BASIS;
      for (int j = 0; j < buckets; j++) {
        long v = hist[j];
        h ^= (v + 1L);
        h *= FNV64_PRIME;
        hist[j] = 0L;
      }
    }

    return h;
  }
}
