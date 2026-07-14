import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    
    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0);
      
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      
      h = 1469598103934665603L;
      int i = 0;
      int len = hist.length;
      
      // Unroll by 4
      for (; i + 3 < len; i += 4) {
        long v0 = hist[i];
        long v1 = hist[i + 1];
        long v2 = hist[i + 2];
        long v3 = hist[i + 3];
        
        h ^= (v0 + 1);
        h *= 1099511628211L;
        h ^= (v1 + 1);
        h *= 1099511628211L;
        h ^= (v2 + 1);
        h *= 1099511628211L;
        h ^= (v3 + 1);
        h *= 1099511628211L;
      }
      
      // Handle remainder
      for (; i < len; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
