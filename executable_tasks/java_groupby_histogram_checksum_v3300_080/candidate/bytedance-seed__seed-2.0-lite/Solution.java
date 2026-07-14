public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    if (buckets <= 0 || iters <= 0) return h;
    long[] hist = new long[buckets];
    final int keyCount = keys.length;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < keyCount; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      for (int j = 0; j < buckets; j++) {
        long v = hist[j];
        h ^= (v + 1);
        h *= 1099511628211L;
        hist[j] = 0;
      }
    }
    return h;
  }
}