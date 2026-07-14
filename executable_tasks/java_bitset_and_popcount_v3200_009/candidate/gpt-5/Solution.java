public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    if (iters <= 0) return 0L;

    final long[] l = left;
    final long[] r = right;
    final int n = l.length;
    long total = 0L;

    int i = 0;
    int n8 = n & ~7; // process in chunks of 8
    while (i < n8) {
      long a0 = l[i] & r[i];
      long a1 = l[i + 1] & r[i + 1];
      long a2 = l[i + 2] & r[i + 2];
      long a3 = l[i + 3] & r[i + 3];
      long a4 = l[i + 4] & r[i + 4];
      long a5 = l[i + 5] & r[i + 5];
      long a6 = l[i + 6] & r[i + 6];
      long a7 = l[i + 7] & r[i + 7];
      int sum = Long.bitCount(a0) + Long.bitCount(a1) + Long.bitCount(a2) + Long.bitCount(a3)
              + Long.bitCount(a4) + Long.bitCount(a5) + Long.bitCount(a6) + Long.bitCount(a7);
      total += sum;
      i += 8;
    }

    for (; i < n; i++) {
      total += Long.bitCount(l[i] & r[i]);
    }

    return total;
  }
}
