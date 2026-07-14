import java.util.Arrays;

public class Solution {
  /**
   * Optimizes the dense histogram aggregation and checksum calculation.
   * 
   * Key optimizations:
   * 1. Array Reuse: Moved the histogram array allocation outside the iteration loop.
   * 2. Loop Unrolling: Unrolled the aggregation loop by a factor of 8 to reduce branch overhead
   *    and improve memory throughput.
   * 3. Efficient Clearing: Used Arrays.fill to reset the histogram array between iterations.
   * 4. Checksum Loop: Maintained the FNV-1a checksum logic while ensuring the loop is tight.
   */
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final long[] hist = new long[buckets];
    final int n = keys.length;
    final long fnvOffsetBasis = 1469598103934665603L;
    final long fnvPrime = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      // Reset histogram for the current iteration
      Arrays.fill(hist, 0);

      // Aggregate values into buckets with loop unrolling
      int i = 0;
      int limit = n - 7;
      for (; i < limit; i += 8) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
        hist[keys[i + 4]] += vals[i + 4];
        hist[keys[i + 5]] += vals[i + 5];
        hist[keys[i + 6]] += vals[i + 6];
        hist[keys[i + 7]] += vals[i + 7];
      }
      // Handle remaining elements
      for (; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      // Calculate FNV-1a checksum over the histogram
      h = fnvOffsetBasis;
      for (int j = 0; j < buckets; j++) {
        h ^= (hist[j] + 1);
        h *= fnvPrime;
      }
    }
    return h;
  }
}