public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    int len = keys.length;
    int[] k = keys;
    int[] v = vals;

    for (int iter = 0; iter < iters; iter++) {
      int i = 0;
      for (; i <= len - 4; i += 4) {
        hist[k[i]] += v[i];
        hist[k[i+1]] += v[i+1];
        hist[k[i+2]] += v[i+2];
        hist[k[i+3]] += v[i+3];
      }
      for (; i < len; i++) {
        hist[k[i]] += v[i];
      }
      
      h = 1469598103934665603L;
      for (int j = 0; j < buckets; j++) {
        long val = hist[j];
        h ^= (val + 1);
        h *= 1099511628211L;
        hist[j] = 0;
      }
    }
    return h;
  }
}