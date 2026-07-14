public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int len = keys.length;
    final int lenMain = len & ~3;
    final int bucketsMain = buckets & ~3;
    
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];
      
      // Unrolled aggregation loop (4x)
      int i = 0;
      for (; i < lenMain; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
      }
      // Handle remainder
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // FNV-1a style checksum
      h = 1469598103934665603L;
      
      // Unrolled checksum loop (4x)
      int j = 0;
      for (; j < bucketsMain; j += 4) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 1] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 2] + 1);
        h *= 1099511628211L;
        h ^= (hist[j + 3] + 1);
        h *= 1099511628211L;
      }
      // Handle remainder
      for (; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}