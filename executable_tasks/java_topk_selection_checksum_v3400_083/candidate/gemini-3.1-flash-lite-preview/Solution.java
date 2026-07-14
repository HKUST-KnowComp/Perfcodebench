import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    for (int i = 0; i < n; i++) {
      // Pack score in high 32 bits (inverted for descending sort) and index in low 32 bits
      // We use (long)Integer.MAX_VALUE - score to allow natural ascending sort to act as descending
      packed[i] = ((long) (Integer.MAX_VALUE - scores[i]) << 32) | i;
    }

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Partial sort: ensure top k are at the beginning
      // Since we need the top k ordered, we use a full sort on the primitive array
      // which is significantly faster than sorting objects.
      Arrays.sort(packed);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        int score = Integer.MAX_VALUE - (int) (val >>> 32);
        int index = (int) (val & 0xffffffffL);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}