import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        // Pack score in high 32 bits, index in low 32 bits.
        // To sort descending by score then ascending by index:
        // We store score as (Integer.MAX_VALUE - score) to use natural ascending sort.
        packed[i] = (((long) (Integer.MAX_VALUE - scores[i])) << 32) | i;
      }
      
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