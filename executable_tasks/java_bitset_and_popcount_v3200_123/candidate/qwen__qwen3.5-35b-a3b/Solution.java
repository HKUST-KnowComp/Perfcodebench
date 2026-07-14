public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long iterCount = 0;
      for (int i = 0; i < left.length; i++) {
        long word = left[i] & right[i];
        iterCount += Long.bitCount(word);
      }
      total += iterCount;
    }
    return total;
  }
}