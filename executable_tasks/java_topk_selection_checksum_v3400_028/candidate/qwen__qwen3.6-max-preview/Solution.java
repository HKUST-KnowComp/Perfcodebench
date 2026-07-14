import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        packed[i] = (((long)~scores[i]) << 32) | i;
      }
      Arrays.sort(packed);
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long p = packed[i];
        int score = ~(int)(p >>> 32);
        h ^= (((long) score) << 32) ^ (p & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}