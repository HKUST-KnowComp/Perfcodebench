public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    long h = 0L;

    // Reuse the histogram across iterations to avoid repeated allocation/GC.
    final long[] hist = new long[buckets];

    for (int iter = 0; iter < iters; iter++) {
      // Build histogram.
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      // Exact checksum over all buckets.
      long hash = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        hash ^= (hist[i] + 1L);
        hash *= 1099511628211L;
      }
      h = hash;

      // Clear histogram for next iteration.
      for (int i = 0; i < buckets; i++) {
        hist[i] = 0L;
      }
    }

    return h;
  }
}
