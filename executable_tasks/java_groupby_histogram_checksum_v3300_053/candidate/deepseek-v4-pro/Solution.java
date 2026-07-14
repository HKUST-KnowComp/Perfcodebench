public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters == 0) return 0L;
    // Build histogram once
    long[] hist = new long[buckets];
    for (int i = 0; i < keys.length; i++) {
      hist[keys[i]] += vals[i];
    }
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int b = 0; b < buckets; b++) {
        final long v = hist[b];
        h ^= (v + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}