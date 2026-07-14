import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0);
      
      int i = 0;
      int len = keys.length;
      int limit = len - 3;
      
      // Unrolled loop for histogram accumulation
      for (; i < limit; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
      }
      
      // Handle remaining elements
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      
      h = 1469598103934665603L;
      int j = 0;
      int histLimit = buckets - 7;
      
      // Unrolled loop for checksum computation
      for (; j < histLimit; j += 8) {
        long v0 = hist[j] + 1;
        long v1 = hist[j + 1] + 1;
        long v2 = hist[j + 2] + 1;
        long v3 = hist[j + 3] + 1;
        long v4 = hist[j + 4] + 1;
        long v5 = hist[j + 5] + 1;
        long v6 = hist[j + 6] + 1;
        long v7 = hist[j + 7] + 1;
        
        h ^= v0;
        h *= 1099511628211L;
        h ^= v1;
        h *= 1099511628211L;
        h ^= v2;
        h *= 1099511628211L;
        h ^= v3;
        h *= 1099511628211L;
        h ^= v4;
        h *= 1099511628211L;
        h ^= v5;
        h *= 1099511628211L;
        h ^= v6;
        h *= 1099511628211L;
        h ^= v7;
        h *= 1099511628211L;
      }
      
      // Handle remaining buckets
      for (; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    
    return h;
  }
}
