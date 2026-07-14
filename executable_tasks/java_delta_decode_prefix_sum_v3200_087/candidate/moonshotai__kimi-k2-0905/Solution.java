public class Solution {
  public static long run(int[] deltas, int iters) {
    int len = deltas.length;
    long total = 0;
    final long mask = 0x9E3779B1L;

    for (int iter = iters; iter-- != 0;) {
      long acc = 0;
      for (int i = 0; i < len; i++) {
        acc += deltas[i];
        total += acc + mask;
      }
    }
    return total;
  }
}