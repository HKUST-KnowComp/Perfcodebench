public class Solution {
  public static long run(int[] deltas, int iters) {
    // Match baseline behavior: if no iterations, result is 0
    if (iters <= 0) return 0L;

    int n = deltas.length;
    long current = 0L;
    long sum = 0L;

    int i = 0;
    int limit = n & ~3; // process in chunks of 4
    while (i < limit) {
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
    }
    while (i < n) {
      current += deltas[i++];
      sum += current;
    }

    sum += 0x9E3779B1L * (long) n;
    return sum;
  }
}
