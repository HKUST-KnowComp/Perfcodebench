public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters <= 0) return 0L;
    // Build histogram once.
    long[] hist = new long[buckets];
    final int len = keys.length;
    for (int i = 0; i < len; i++) {
      hist[keys[i]] += vals[i];
    }
    // Compute checksum once.
    long h = 1469598103934665603L;
    for (int b = 0; b < buckets; b++) {
      long v = hist[b];
      h ^= (v + 1);
      h *= 1099511628211L;
    }
    return h;
  }
}