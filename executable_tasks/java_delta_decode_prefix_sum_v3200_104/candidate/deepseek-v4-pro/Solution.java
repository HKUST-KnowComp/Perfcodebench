public class Solution {
  public static long run(int[] deltas, int iters) {
    long sum = 0;
    long current = 0;
    for (int delta : deltas) {
      current += delta;
      sum += current;
    }
    long total_one = sum + deltas.length * 0x9E3779B1L;
    return total_one * iters;
  }
}
