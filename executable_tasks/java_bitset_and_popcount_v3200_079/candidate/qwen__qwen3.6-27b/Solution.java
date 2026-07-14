public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    if (iters <= 0) {
      return 0;
    }
    long count = 0;
    int len = left.length;
    for (int i = 0; i < len; i++) {
      count += Long.bitCount(left[i] & right[i]);
    }
    return count;
  }
}