public class Solution {
  public static long run(int[] deltas, int iters) {
    // If no iterations requested, match baseline behavior (returns 0)
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    long current = 0L;
    long sum = 0L;

    // Unrolled loop for fewer branches and better throughput
    int i = 0;
    int limit = n - (n & 3);
    while (i < limit) {
      current += deltas[i];
      sum += current;
      current += deltas[i + 1];
      sum += current;
      current += deltas[i + 2];
      sum += current;
      current += deltas[i + 3];
      sum += current;
      i += 4;
    }
    while (i < n) {
      current += deltas[i++];
      sum += current;
    }

    // Add the constant per element once
    return sum + (0x9E3779B1L * (long) n);
  }
}
