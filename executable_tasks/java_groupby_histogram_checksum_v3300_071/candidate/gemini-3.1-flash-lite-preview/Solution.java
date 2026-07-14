public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    int n = keys.length;
    long[] hist = new long[buckets];
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram efficiently
      java.util.Arrays.fill(hist, 0L);
      
      // Aggregate values
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Compute checksum
      long currentH = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        currentH ^= (hist[i] + 1);
        currentH *= 1099511628211L;
      }
      h = currentH;
    }
    return h;
  }
}