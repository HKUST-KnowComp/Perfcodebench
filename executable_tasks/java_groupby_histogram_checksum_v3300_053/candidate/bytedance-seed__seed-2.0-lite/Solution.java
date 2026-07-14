public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    // Allocate histogram once to avoid repeated allocation overhead
    long[] hist = new long[buckets];
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    for (int iter = 0; iter < iters; iter++) {
      // Reset reused array instead of allocating new, far lower overhead
      for (int b = 0; b < buckets; b++) {
        hist[b] = 0;
      }
      // Original accumulation logic preserved for correctness
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      // Exact same checksum calculation as baseline to maintain correctness
      h = FNV_OFFSET;
      for (long v : hist) {
        h ^= (v + 1);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}