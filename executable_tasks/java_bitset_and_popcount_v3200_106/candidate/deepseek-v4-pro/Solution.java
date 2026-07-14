public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long sum = 0;
    for (int i = 0; i < left.length; i++) {
      sum += Long.bitCount(left[i] & right[i]);
    }
    return sum;
  }
}