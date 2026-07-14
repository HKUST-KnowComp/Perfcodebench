public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    int len = keys.length;
    int limit = len - 7;
    long[] hist = new long[buckets];
    for (int iter = 0; iter < iters; iter++) {
      java.util.Arrays.fill(hist, 0L);
      int i = 0;
      for (; i <= limit; i += 8) {
        hist[keys[i]] += vals[i];
        hist[keys[i+1]] += vals[i+1];
        hist[keys[i+2]] += vals[i+2];
        hist[keys[i+3]] += vals[i+3];
        hist[keys[i+4]] += vals[i+4];
        hist[keys[i+5]] += vals[i+5];
        hist[keys[i+6]] += vals[i+6];
        hist[keys[i+7]] += vals[i+7];
      }
      for (; i < len; i++) {
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