public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters == 0) return 0;
    
    long[] hist = new long[buckets];
    for (int i = 0; i < keys.length; i++) {
      hist[keys[i]] += vals[i];
    }
    
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int j = 0; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}