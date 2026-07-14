public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    int n = deltas.length;
    long current = 0L;
    long sum = 0L;

    int i = 0;
    int limit = n - (n & 7); // unroll by 8
    for (; i < limit; i += 8) {
      current += deltas[i];
      sum += current;
      current += deltas[i + 1];
      sum += current;
      current += deltas[i + 2];
      sum += current;
      current += deltas[i + 3];
      sum += current;
      current += deltas[i + 4];
      sum += current;
      current += deltas[i + 5];
      sum += current;
      current += deltas[i + 6];
      sum += current;
      current += deltas[i + 7];
      sum += current;
    }
    for (; i < n; i++) {
      current += deltas[i];
      sum += current;
    }

    sum += 0x9E3779B1L * (long) n;
    return sum;
  }
}
