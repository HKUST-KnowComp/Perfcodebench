public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    final long[] hist = new long[buckets];
    final int[] touched = new int[buckets];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int touchedCount = 0;

      for (int i = 0; i < n; i++) {
        final int k = keys[i];
        if (hist[k] == 0L) {
          touched[touchedCount++] = k;
        }
        hist[k] += vals[i];
      }

      long hash = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        hash ^= (hist[i] + 1L);
        hash *= 1099511628211L;
      }
      h = hash;

      for (int i = 0; i < touchedCount; i++) {
        hist[touched[i]] = 0L;
      }
    }

    return h;
  }
}
