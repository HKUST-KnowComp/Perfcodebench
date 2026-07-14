import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    for (int i = 0; i < n; i++) {
      // Pack score in high 32 bits, index in low 32 bits.
      // To sort descending by score, we negate the score or use a custom comparator.
      // Since we need to sort by score desc, then index asc, we store:
      // (long)score << 32 | (0xFFFFFFFFL - index)
      // This allows standard ascending sort to achieve the desired order.
      packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL - i);
    }

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Re-initialize packed array for each iteration
      for (int i = 0; i < n; i++) {
        packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL - i);
      }
      
      Arrays.sort(packed);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[n - 1 - i];
        int score = (int) (val >>> 32);
        int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
        
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}