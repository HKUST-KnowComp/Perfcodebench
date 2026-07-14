public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    // Match baseline behavior: if no iterations, return initial h (0)
    if (iters <= 0) return 0L;

    // Build histogram once since each iteration would yield identical result
    long[] hist = new long[buckets];
    int[] k = keys;
    int[] v = vals;
    int n = k.length;
    for (int i = 0; i < n; i++) {
      hist[k[i]] += v[i];
    }

    // Compute checksum once (same as the baseline's last iteration result)
    long h = 1469598103934665603L; // FNV offset basis
    for (int i = 0; i < buckets; i++) {
      long val = hist[i];
      h ^= (val + 1L);
      h *= 1099511628211L; // FNV prime
    }
    return h;
  }
}
