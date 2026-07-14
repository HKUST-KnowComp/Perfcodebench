import java.util.*;

public class Solution {
  /**
   * Optimized implementation using primitive long packing to avoid object allocation.
   * We pack the score and index into a single long to allow efficient sorting.
   * Since we need the top-k in a specific order (descending score, then ascending index),
   * we transform the score to allow a single natural sort order.
   */
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // We want descending score, then ascending index.
        // To use Arrays.sort (ascending), we transform the score:
        // 1. Flip the sign of the score (or use its complement) to reverse order.
        // 2. Use the index as the lower 32 bits.
        // However, to handle the 'ascending index' requirement with 'descending score',
        // we can map score -> (Integer.MAX_VALUE - score) or similar.
        // A more robust way for all integers: 
        // High 32 bits: (score ^ Integer.MIN_VALUE) [this flips the sign bit, making larger scores smaller]
        // Low 32 bits: index
        // Wait, if we want descending score, we want the largest score to be the smallest value in the long.
        // Let's use: high 32 bits = ~score, low 32 bits = index.
        // If score A > score B, then ~A < ~B. 
        // If ~A == ~B (same score), then index A < index B is preserved.
        long s = scores[i];
        long invertedScore = ~s;
        packed[i] = (invertedScore << 32) | (i & 0xFFFFFFFFL);
      }

      Arrays.sort(packed);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        int originalScore = (int) ~(val >>> 32);
        int originalIndex = (int) (val & 0xFFFFFFFFL);
        
        h ^= (((long) originalScore) << 32) ^ (originalIndex & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}