public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    long cur = 0L;
    long sum = 0L;

    int i = 0;
    int limit = n & ~3; // largest multiple of 4 <= n
    while (i < limit) {
      cur += deltas[i++];
      sum += cur;
      cur += deltas[i++];
      sum += cur;
      cur += deltas[i++];
      sum += cur;
      cur += deltas[i++];
      sum += cur;
    }
    while (i < n) {
      cur += deltas[i++];
      sum += cur;
    }

    sum += 0x9E3779B1L * (long) n;
    return sum;
  }
}
