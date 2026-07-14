public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentTotal = 0;
      int i = 0;
      for (; i <= len - 8; i += 8) {
        currentTotal += Long.bitCount(left[i] & right[i]);
        currentTotal += Long.bitCount(left[i+1] & right[i+1]);
        currentTotal += Long.bitCount(left[i+2] & right[i+2]);
        currentTotal += Long.bitCount(left[i+3] & right[i+3]);
        currentTotal += Long.bitCount(left[i+4] & right[i+4]);
        currentTotal += Long.bitCount(left[i+5] & right[i+5]);
        currentTotal += Long.bitCount(left[i+6] & right[i+6]);
        currentTotal += Long.bitCount(left[i+7] & right[i+7]);
      }
      for (; i < len; i++) {
        currentTotal += Long.bitCount(left[i] & right[i]);
      }
      total = currentTotal;
    }
    return total;
  }
}
