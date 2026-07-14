public class Solution {
  public static long run(int[] deltas, int iters) {
    long[] values = new long[deltas.length];
    long current = 0;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      values[i] = current;
    }
    long sumPerIter = 0;
    for (long v : values) {
      sumPerIter += v + 0x9E3779B1L;
    }
    return sumPerIter * iters;
  }
}