public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    long[] l = left;
    long[] r = right;
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      for (int i = 0; i < len; i++) {
        total += Long.bitCount(l[i] & r[i]);
      }
    }
    return total;
  }
}