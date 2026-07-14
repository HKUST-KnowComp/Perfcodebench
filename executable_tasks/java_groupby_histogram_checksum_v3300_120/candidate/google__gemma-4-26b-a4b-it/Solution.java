public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    int n = keys.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram efficiently
      java.util.Arrays.fill(hist, 0L);
      
      // Aggregation loop
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // FNV-1a style checksum calculation
      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}