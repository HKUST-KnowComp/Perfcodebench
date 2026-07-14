public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long[] l = left;
    long[] r = right;
    int n = l.length; // assume same length as right, as in baseline

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long s0 = 0L, s1 = 0L, s2 = 0L, s3 = 0L;
      int i = 0;
      int limit = n & ~3; // process in blocks of 4

      for (; i < limit; i += 4) {
        s0 += Long.bitCount(l[i] & r[i]);
        s1 += Long.bitCount(l[i + 1] & r[i + 1]);
        s2 += Long.bitCount(l[i + 2] & r[i + 2]);
        s3 += Long.bitCount(l[i + 3] & r[i + 3]);
      }

      long s = (s0 + s1) + (s2 + s3);

      for (; i < n; i++) {
        s += Long.bitCount(l[i] & r[i]);
      }

      total = s; // match baseline semantics: result from the last iteration
    }

    return total;
  }
}
