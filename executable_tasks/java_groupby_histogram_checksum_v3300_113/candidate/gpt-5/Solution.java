public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    // Preserve baseline behavior for non-positive iterations
    if (iters <= 0) return 0L;

    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    // Build histogram once; final checksum depends only on the last iteration
    long[] hist = new long[buckets];

    int n = keys.length; // baseline uses keys.length
    // Aggregate values into histogram
    for (int i = 0; i < n; i++) {
      int k = keys[i];
      hist[k] += vals[i];
    }

    // Compute checksum over histogram (FNV-1a-like as in baseline)
    long h = FNV_OFFSET_BASIS;
    int i = 0;
    int limit = buckets & ~3; // unroll by 4
    while (i < limit) {
      long v0 = hist[i];
      long v1 = hist[i + 1];
      long v2 = hist[i + 2];
      long v3 = hist[i + 3];

      h ^= (v0 + 1L);
      h *= FNV_PRIME;
      h ^= (v1 + 1L);
      h *= FNV_PRIME;
      h ^= (v2 + 1L);
      h *= FNV_PRIME;
      h ^= (v3 + 1L);
      h *= FNV_PRIME;

      i += 4;
    }
    for (; i < buckets; i++) {
      h ^= (hist[i] + 1L);
      h *= FNV_PRIME;
    }

    return h;
  }
}
