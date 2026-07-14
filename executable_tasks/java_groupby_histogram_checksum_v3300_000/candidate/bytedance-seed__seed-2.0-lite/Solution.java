import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    if (iters <= 0) {
      return h;
    }
    long[] hist = new long[buckets];
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      for (long v : hist) {
        h ^= (v + 1);
        h *= 1099511628211L;
      }
      if (iter != iters - 1) {
        Arrays.fill(hist, 0L);
      }
    }
    return h;
  }
}