public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int len = keys.length;
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];
      for (int i = 0; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      
      long currentH = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        currentH ^= (hist[i] + 1);
        currentH *= 1099511628211L;
      }
      h = currentH;
    }
    return h;
  }
}