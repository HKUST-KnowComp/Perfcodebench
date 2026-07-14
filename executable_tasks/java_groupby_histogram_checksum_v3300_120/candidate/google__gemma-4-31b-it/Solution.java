public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram for each iteration
      java.util.Arrays.fill(hist, 0L);
      
      // Aggregate values into buckets
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      
      // Calculate checksum
      h = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}