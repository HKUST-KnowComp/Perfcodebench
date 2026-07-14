import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int[] k = keys;
    final int[] v = vals;
    final int n = k.length;
    final long[] hist = new long[buckets];

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0L);

      for (int i = 0; i < n; i++) {
        hist[k[i]] += v[i];
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
