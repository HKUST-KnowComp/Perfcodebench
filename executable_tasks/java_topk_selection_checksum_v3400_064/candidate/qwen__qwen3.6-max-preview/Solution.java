import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    long h = 0;
    final long HIGH_MASK = 0xFFFFFFFF00000000L;
    final long LOW_MASK = 0xFFFFFFFFL;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        packed[i] = ((long) scores[i] << 32) | (~i & LOW_MASK);
      }
      Arrays.sort(packed);
      h = 1469598103934665603L;
      for (int j = n - 1; j >= n - k; j--) {
        long p = packed[j];
        h ^= (p & HIGH_MASK) ^ (~(int)p & LOW_MASK);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}