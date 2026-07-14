public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    int len = keys.length;
    for (int iter = 0; iter < iters; iter++) {
      java.util.Arrays.fill(hist, 0L);
      for (int i = 0; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      long hv = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        long v = hist[i];
        hv ^= (v + 1);
        hv *= 1099511628211L;
      }
      h = hv;
    }
    return h;
  }
}