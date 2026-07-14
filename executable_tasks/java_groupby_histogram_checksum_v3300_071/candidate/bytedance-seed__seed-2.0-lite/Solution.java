public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    
    for (int iter = 0; iter < iters; iter++) {
      // Populate histogram from input keys/vals
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      // Compute checksum and reset buckets in single pass
      h = FNV_OFFSET;
      for (int b = 0; b < buckets; b++) {
        long v = hist[b];
        h ^= (v + 1);
        h *= FNV_PRIME;
        hist[b] = 0; // Reset for next iteration
      }
    }
    return h;
  }
}