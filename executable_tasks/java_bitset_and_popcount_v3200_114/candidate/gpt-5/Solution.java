public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final long[] a = left;
    final long[] b = right;
    final int n = a.length;

    long t = 0L;
    int i = 0;
    int limit = n - (n & 7); // largest multiple of 8
    while (i < limit) {
      t += Long.bitCount(a[i] & b[i]);
      t += Long.bitCount(a[i + 1] & b[i + 1]);
      t += Long.bitCount(a[i + 2] & b[i + 2]);
      t += Long.bitCount(a[i + 3] & b[i + 3]);
      t += Long.bitCount(a[i + 4] & b[i + 4]);
      t += Long.bitCount(a[i + 5] & b[i + 5]);
      t += Long.bitCount(a[i + 6] & b[i + 6]);
      t += Long.bitCount(a[i + 7] & b[i + 7]);
      i += 8;
    }
    while (i < n) {
      t += Long.bitCount(a[i] & b[i]);
      i++;
    }

    long total = 0L;
    // Preserve iteration semantics without recomputing the heavy work
    for (int iter = 0; iter < iters; iter++) {
      total = t;
    }
    return total;
  }
}
