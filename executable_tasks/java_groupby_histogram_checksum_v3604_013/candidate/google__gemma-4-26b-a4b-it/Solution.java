import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    int n = keys.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset the histogram instead of reallocating
      Arrays.fill(hist, 0L);
      
      // Aggregation loop
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Checksum calculation
      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}