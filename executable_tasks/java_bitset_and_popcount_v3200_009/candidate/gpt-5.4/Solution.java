public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final long[] l = left;
    final long[] r = right;
    final int n = l.length;

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      for (int i = 0; i < n; i++) {
        sum += Long.bitCount(l[i] & r[i]);
      }
      total = sum;
    }
    return total;
  }
}
