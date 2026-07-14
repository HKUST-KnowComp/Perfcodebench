import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    
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
      
      // Compute checksum with unrolled loop
      h = 1469598103934665603L;
      int j = 0;
      int histLen = hist.length;
      int histLimit = histLen - 3;
      
      for (; j < histLimit; j += 4) {
        long v0 = hist[j];
        long v1 = hist[j + 1];
        long v2 = hist[j + 2];
        long v3 = hist[j + 3];
        
        h ^= (v0 + 1);
        h *= 1099511628211L;
        h ^= (v1 + 1);
        h *= 1099511628211L;
        h ^= (v2 + 1);
        h *= 1099511628211L;
        h ^= (v3 + 1);
        h *= 1099511628211L;
      }
      
      // Handle remaining buckets
      for (; j < histLen; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
