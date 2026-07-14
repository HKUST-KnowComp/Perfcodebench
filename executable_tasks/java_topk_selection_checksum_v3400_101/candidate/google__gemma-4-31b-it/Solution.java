import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score and index into a long for efficient sorting.
        // We want descending score, then ascending index.
        // To achieve this with a standard ascending sort:
        // 1. Invert the score bits (or subtract from max) to make larger scores smaller.
        // 2. Store the index in the lower 32 bits.
        // Since scores are ints, we can use ~score to flip the order.
        packed[i] = (((long) ~scores[i]) << 32) | (i & 0xFFFFFFFFL);
      }
      
      Arrays.sort(packed);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        int score = (int) ~(val >> 32);
        int index = (int) (val & 0xFFFFFFFFL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}