public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    int keysLen = keys.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram array
      java.util.Arrays.fill(hist, 0L);
      
      // Aggregate values
      for (int i = 0; i < keysLen; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Calculate checksum
      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}