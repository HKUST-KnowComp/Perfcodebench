public class Solution {
  public static long run(int[] deltas, int iters) {
    long current = 0;
    long sumPerIter = 0;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      sumPerIter += current + 0x9E3779B1L;
    }
    return sumPerIter * iters;
  }
}