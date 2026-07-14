import java.util.Arrays;

public class Solution {
  /**
   * Optimizes the top-k selection by packing score and index into a single long.
   * Score is placed in the upper 32 bits, and (Integer.MAX_VALUE - index) in the lower 32 bits.
   * This allows a standard descending sort to prioritize higher scores, and for equal scores,
   * prioritize lower indices.
   */
  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    final long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      // Pack score and index into a long for efficient sorting without object overhead.
      // Using (Integer.MAX_VALUE - i) ensures that for identical scores, 
      // the smaller index results in a larger packed value.
      for (int i = 0; i < n; i++) {
        packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL & (Integer.MAX_VALUE - i));
      }

      // Sort the primitive array. Arrays.sort for primitives is highly optimized.
      Arrays.sort(packed);

      // Reset checksum for this iteration.
      h = 1469598103934665603L;
      
      // Arrays.sort sorts in ascending order. The top-k winners are at the end of the array.
      // We iterate backwards from the end to get the top-k in descending order.
      for (int i = 0; i < k; i++) {
        long val = packed[n - 1 - i];
        int score = (int) (val >> 32);
        // Recover the original index from the inverted storage.
        int index = Integer.MAX_VALUE - (int) (val & 0xFFFFFFFFL);

        // FNV-1a checksum calculation.
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}