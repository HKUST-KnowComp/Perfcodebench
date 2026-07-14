public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long perIterSum = 0;
    for (int i = 0; i < left.length; i++) {
      perIterSum += Long.bitCount(left[i] & right[i]);
    }
    return perIterSum * iters;
  }
}