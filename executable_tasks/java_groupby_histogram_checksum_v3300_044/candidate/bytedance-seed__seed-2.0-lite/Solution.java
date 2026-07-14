public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int n = keys.length;
    final int b = buckets;
    long[] hist = new long[b];
    
    for (int iter = 0; iter < iters; iter++) {
      // Aggregate values into histogram
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Compute FNV-1a checksum
      h = FNV_OFFSET;
      for (long v : hist) {
        h ^= (v + 1);
        h *= FNV_PRIME;
      }
      
      // Reset histogram for next iteration, only touching used keys
      for (int i = 0; i < n; i++) {
        hist[keys[i]] -= vals[i];
      }
    }
    return h;
  }
}