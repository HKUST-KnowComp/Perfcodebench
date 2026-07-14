public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long currentSum = 0;
      for (int i = 0; i < left.length; i++) {
        long andWord = left[i] & right[i];
        currentSum += Long.bitCount(andWord);
      }
      total = currentSum;
    }
    return total;
  }
}