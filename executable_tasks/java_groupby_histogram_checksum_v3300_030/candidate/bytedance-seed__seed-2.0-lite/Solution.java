public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    // Allocate histogram once to avoid repeated allocation/GC costs
    long[] hist = new long[buckets];
    final int keysLen = keys.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Accumulate values into histogram, same logic as baseline
      for (int i = 0; i < keysLen; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Initialize FNV-1a checksum exactly as baseline
      h = 1469598103934665603L;
      // Fuse checksum calculation and histogram reset into single pass
      for (int j = 0; j < buckets; j++) {
        long v = hist[j];
        h ^= (v + 1);
        h *= 1099511628211L;
        hist[j] = 0; // Reset bucket for next iteration
      }
    }
    return h;
  }
}