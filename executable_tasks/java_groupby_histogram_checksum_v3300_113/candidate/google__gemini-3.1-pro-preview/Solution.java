import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int n = keys.length;
    // Reuse the histogram array to avoid allocation and GC pressure inside the loop.
    long[] hist = new long[buckets];
    
    final long fnvOffsetBasis = 1469598103934665603L;
    final long fnvPrime = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      // Efficiently zero out the array.
      Arrays.fill(hist, 0L);

      // Unroll the aggregation loop to improve throughput.
      int i = 0;
      for (; i <= n - 4; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
      }
      for (; i < n; i++) {
        hist[keys[i]] += vals[i];
      }

      // Checksum calculation
      h = fnvOffsetBasis;
      
      // Unroll the checksum loop.
      int j = 0;
      for (; j <= buckets - 4; j += 4) {
        h = (h ^ (hist[j] + 1)) * fnvPrime;
        h = (h ^ (hist[j + 1] + 1)) * fnvPrime;
        h = (h ^ (hist[j + 2] + 1)) * fnvPrime;
        h = (h ^ (hist[j + 3] + 1)) * fnvPrime;
      }
      for (; j < buckets; j++) {
        h = (h ^ (hist[j] + 1)) * fnvPrime;
      }
    }
    return h;
  }
}