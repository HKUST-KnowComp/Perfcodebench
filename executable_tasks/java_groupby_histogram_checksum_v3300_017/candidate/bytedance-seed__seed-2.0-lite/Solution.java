import java.util.Arrays;
public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    // Allocate histogram once to avoid repeated allocation/GC overhead
    long[] hist = new long[buckets];
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    final int keyCount = keys.length;
    
    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram to zero, far cheaper than new array allocation
      Arrays.fill(hist, 0L);
      // Aggregate keyed values into histogram
      for (int i = 0; i < keyCount; i++) {
        hist[keys[i]] += vals[i];
      }
      // Compute exact same FNV-1a checksum as baseline
      h = FNV_OFFSET;
      for (int i = 0; i < buckets; i++) {
        long v = hist[i];
        h ^= (v + 1);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}