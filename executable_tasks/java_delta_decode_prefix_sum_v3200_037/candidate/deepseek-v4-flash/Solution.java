public class Solution {
  public static long run(int[] deltas, int iters) {
    long prefix = 0;
    long sum = 0;
    for (int delta : deltas) {
      prefix += delta;
      sum += prefix;
    }
    long total = sum + (long)deltas.length * 0x9E3779B1L;
    return total;
  }
}
