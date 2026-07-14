public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int len = keys.length;
    final int len4 = len - 3;
    final int buckets4 = buckets - 3;
    
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];
      
      int i = 0;
      for (; i < len4; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
      }
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      
      h = 1469598103934665603L;
      int j = 0;
      for (; j < buckets4; j += 4) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 1] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 2] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 3] + 1);
        h *= 1099511628211L;
      }
      for (; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}