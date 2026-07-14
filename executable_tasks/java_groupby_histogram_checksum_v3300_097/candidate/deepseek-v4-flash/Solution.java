public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    int len = keys.length;
    int b = buckets;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      long h = 1469598103934665603L;
      for (int i = 0; i < b; i++) {
        long v = hist[i];
        h ^= (v + 1);
        h *= 1099511628211L;
        hist[i] = 0L;
      }
    }
    return h;
  }
}