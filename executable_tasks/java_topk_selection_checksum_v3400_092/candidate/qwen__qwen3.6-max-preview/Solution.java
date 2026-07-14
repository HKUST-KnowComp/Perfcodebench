import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] work = new long[n];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        work[i] = (((long) scores[i]) << 32) | (~i & 0xffffffffL);
      }
      Arrays.sort(work);
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = work[n - 1 - i] ^ 0xffffffffL;
        h ^= val;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}