public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    // If no iterations requested, baseline returns 0
    if (iters <= 0) return 0L;

    // Compute histogram once; the final checksum only depends on the bucket sums
    final long[] hist = new long[buckets];
    final int n = keys.length;
    for (int i = 0; i < n; i++) {
      hist[keys[i]] += vals[i];
    }

    // Compute FNV-1a-like checksum over the buckets
    long h = 1469598103934665603L;
    final long prime = 1099511628211L;
    for (int i = 0; i < buckets; i++) {
      h ^= (hist[i] + 1L);
      h *= prime;
    }

    return h;
  }
}
