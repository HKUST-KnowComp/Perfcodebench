public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumOfValues = 0;
      for (int delta : deltas) {
        current += delta;
        sumOfValues += current;
      }
      total += sumOfValues + (iters * 0x9E3779B1L);
    }
    return total;
  }
}