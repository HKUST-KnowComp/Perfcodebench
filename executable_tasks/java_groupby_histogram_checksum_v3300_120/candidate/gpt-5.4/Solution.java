public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    final long[] hist = new long[buckets];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      java.util.Arrays.fill(hist, 0L);

      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      long hash = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        hash ^= (hist[i] + 1L);
        hash *= 1099511628211L;
      }
      h = hash;
    }

    return h;
  }
}
