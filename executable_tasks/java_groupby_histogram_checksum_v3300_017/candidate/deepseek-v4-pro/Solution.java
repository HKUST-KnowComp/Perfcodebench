import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    int nKeys = keys.length;
    long[] hist = new long[buckets];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0);
      for (int i = 0; i < nKeys; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      for (int b = 0; b < buckets; b++) {
        long v = hist[b];
        h ^= (v + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}