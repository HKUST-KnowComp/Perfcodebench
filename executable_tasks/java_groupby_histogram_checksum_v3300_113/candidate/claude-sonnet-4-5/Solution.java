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
      
      // Unrolled loop for better ILP
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
      int histLimit = buckets - 3;
      
      // Unrolled checksum computation
      for (; j < histLimit; j += 4) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 1] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 2] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 3] + 1);
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