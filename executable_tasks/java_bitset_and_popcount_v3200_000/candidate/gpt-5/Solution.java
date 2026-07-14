public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    // Preserve baseline behavior: if iters == 0, return 0
    if (iters <= 0) return 0L;

    final long[] a = left;
    final long[] b = right;
    final int n = a.length;

    long s0 = 0L, s1 = 0L, s2 = 0L, s3 = 0L;
    int i = 0;
    final int limit = n & ~3; // process in chunks of 4

    while (i < limit) {
      s0 += Long.bitCount(a[i] & b[i]);
      s1 += Long.bitCount(a[i + 1] & b[i + 1]);
      s2 += Long.bitCount(a[i + 2] & b[i + 2]);
      s3 += Long.bitCount(a[i + 3] & b[i + 3]);
      i += 4;
    }

    long total = s0 + s1 + s2 + s3;
    for (; i < n; i++) {
      total += Long.bitCount(a[i] & b[i]);
    }

    return total;
  }
}
