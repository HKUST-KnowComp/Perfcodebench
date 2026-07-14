public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long[] a = left;
    long[] b = right; // baseline assumes equal length
    int len = a.length;
    long sum = 0L;

    int i = 0;
    int limit = len - (len & 3);
    while (i < limit) {
      sum += Long.bitCount(a[i] & b[i])
           + Long.bitCount(a[i + 1] & b[i + 1])
           + Long.bitCount(a[i + 2] & b[i + 2])
           + Long.bitCount(a[i + 3] & b[i + 3]);
      i += 4;
    }
    while (i < len) {
      sum += Long.bitCount(a[i] & b[i]);
      i++;
    }

    return sum;
  }
}