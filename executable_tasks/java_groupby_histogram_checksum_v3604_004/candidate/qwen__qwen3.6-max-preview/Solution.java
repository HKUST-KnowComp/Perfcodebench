public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    int n = keys.length;
    int limit = n - (n % 4);
    for (int iter = 0; iter < iters; iter++) {
      java.util.Arrays.fill(hist, 0);
      for (int i = 0; i < limit; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i+1]] += vals[i+1];
        hist[keys[i+2]] += vals[i+2];
        hist[keys[i+3]] += vals[i+3];
      }
      for (int i = limit; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}