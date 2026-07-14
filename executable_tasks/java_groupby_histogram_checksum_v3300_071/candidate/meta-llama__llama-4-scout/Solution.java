public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 1469598103934665603L;
    long[] hist = new long[buckets];
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < keys.length; i++) hist[keys[i]] += vals[i];
      long tempH = h;
      for (long v : hist) {
        tempH ^= (v + 1);
        tempH *= 1099511628211L;
      }
      h = tempH;
    }
    return h;
  }
}