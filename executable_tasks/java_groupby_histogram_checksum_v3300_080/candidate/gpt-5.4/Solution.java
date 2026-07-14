public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    final long[] hist = new long[buckets];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      java.util.Arrays.fill(hist, 0L);

      int i = 0;
      int limit = n - 7;
      while (i < limit) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
        hist[keys[i + 4]] += vals[i + 4];
        hist[keys[i + 5]] += vals[i + 5];
        hist[keys[i + 6]] += vals[i + 6];
        hist[keys[i + 7]] += vals[i + 7];
        i += 8;
      }
      while (i < n) {
        hist[keys[i]] += vals[i];
        i++;
      }

      long hash = 1469598103934665603L;
      for (int b = 0; b < buckets; b++) {
        hash ^= (hist[b] + 1L);
        hash *= 1099511628211L;
      }
      h = hash;
    }

    return h;
  }
}
