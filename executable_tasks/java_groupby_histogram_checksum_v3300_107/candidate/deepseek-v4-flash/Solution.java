public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    final long prime = 1099511628211L;
    final long offset = 1469598103934665603L;
    long[] hist = new long[buckets];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int j = 0; j < buckets; j++) {
        hist[j] = 0L;
      }
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      h = offset;
      for (int j = 0; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= prime;
      }
    }
    return h;
  }
}