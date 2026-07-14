public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    int len = left.length;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      int i = 0;
      long s0 = 0, s1 = 0, s2 = 0, s3 = 0, s4 = 0, s5 = 0, s6 = 0, s7 = 0;
      for (; i + 7 < len; i += 8) {
        s0 += Long.bitCount(left[i] & right[i]);
        s1 += Long.bitCount(left[i + 1] & right[i + 1]);
        s2 += Long.bitCount(left[i + 2] & right[i + 2]);
        s3 += Long.bitCount(left[i + 3] & right[i + 3]);
        s4 += Long.bitCount(left[i + 4] & right[i + 4]);
        s5 += Long.bitCount(left[i + 5] & right[i + 5]);
        s6 += Long.bitCount(left[i + 6] & right[i + 6]);
        s7 += Long.bitCount(left[i + 7] & right[i + 7]);
      }
      long sum = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
      for (; i < len; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }
      total = sum;
    }
    return total;
  }
}