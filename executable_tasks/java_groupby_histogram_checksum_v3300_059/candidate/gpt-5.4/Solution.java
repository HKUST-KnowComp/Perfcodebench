public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    final long[] hist = new long[buckets];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        long v = hist[i];
        h ^= (v + 1L);
        h *= 1099511628211L;
        hist[i] = 0L;
      }
    }
    return h;
  }
}
