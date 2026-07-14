public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < left.length; i++) {
        long word = left[i] & right[i];
        while (word != 0) {
          total += word & 1;
          word &= word - 1;
        }
      }
    }
    return total;
  }
}