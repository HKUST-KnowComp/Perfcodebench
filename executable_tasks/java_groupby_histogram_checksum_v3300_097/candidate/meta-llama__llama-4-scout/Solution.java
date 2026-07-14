public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < hist.length; i++) {
        hist[i] = 0;
      }
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      for (long v : hist) {
        h ^= (v + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}