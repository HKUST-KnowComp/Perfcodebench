import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    long[] packed = new long[n];
    for (int i = 0; i < n; i++) {
      packed[i] = ((long)scores[i] << 32) | (0xFFFFFFFFL - i);
    }
    Arrays.sort(packed);

    long h = 1469598103934665603L;
    int limit = Math.min(k, n);
    for (int i = 0; i < limit; i++) {
      long p = packed[n - 1 - i];
      long score = p >>> 32;
      long index = 0xFFFFFFFFL - (p & 0xFFFFFFFFL);
      h ^= (score << 32) ^ index;
      h *= 1099511628211L;
    }
    return h;
  }
}