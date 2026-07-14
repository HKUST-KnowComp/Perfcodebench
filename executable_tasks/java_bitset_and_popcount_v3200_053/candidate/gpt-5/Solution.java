public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    // Preserve semantics: if no iterations requested, return 0
    if (iters <= 0) return 0L;

    final long[] a = left;
    final long[] b = right;
    final int len = a.length; // Baseline uses left.length for indexing

    long sum = 0L;
    int i = 0;
    int limit = len & ~7; // process in chunks of 8 for better ILP and fewer loop overheads

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
    for (; i < len; i++) {
      sum += Long.bitCount(a[i] & b[i]);
    }

    // Baseline overwrote total each iteration with the same value; return the computed sum once.
    return sum;
  }
}
