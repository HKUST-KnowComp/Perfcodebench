import java.util.Arrays;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    long[] packed = new long[scores.length];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < scores.length; i++) {
        packed[i] = ((long)(~scores[i]) << 32) | (i & 0xFFFFFFFFL);
      }
      Arrays.sort(packed);
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long val = packed[i];
        long score = ~(val >>> 32);
        long index = val & 0xFFFFFFFFL;
        h ^= ((score << 32) ^ index);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}