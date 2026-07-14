public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    int n = keys.length;
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];
      int i = 0;
      for (; i <= n - 8; i += 8) {
        hist[keys[i]] += vals[i];
        hist[keys[i+1]] += vals[i+1];
        hist[keys[i+2]] += vals[i+2];
        hist[keys[i+3]] += vals[i+3];
        hist[keys[i+4]] += vals[i+4];
        hist[keys[i+5]] += vals[i+5];
        hist[keys[i+6]] += vals[i+6];
        hist[keys[i+7]] += vals[i+7];
      }
      for (; i < n; i++) {
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