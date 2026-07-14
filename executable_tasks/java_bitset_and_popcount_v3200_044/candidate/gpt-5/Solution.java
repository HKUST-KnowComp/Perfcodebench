public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    if (iters <= 0) return 0L;

    final long[] a = left;
    final long[] b = right; // assume same length as baseline behavior
    final int n = a.length;

    long sum = 0L;
    int i = 0;
    int limit = n - (n & 7); // unroll by 8
    for (; i < limit; i += 8) {
      sum += Long.bitCount(a[i] & b[i]);
      sum += Long.bitCount(a[i + 1] & b[i + 1]);
      sum += Long.bitCount(a[i + 2] & b[i + 2]);
      sum += Long.bitCount(a[i + 3] & b[i + 3]);
      sum += Long.bitCount(a[i + 4] & b[i + 4]);
      sum += Long.bitCount(a[i + 5] & b[i + 5]);
      sum += Long.bitCount(a[i + 6] & b[i + 6]);
      sum += Long.bitCount(a[i + 7] & b[i + 7]);
    }
    for (; i < n; i++) {
      sum += Long.bitCount(a[i] & b[i]);
    }

    return sum;
  }
}
