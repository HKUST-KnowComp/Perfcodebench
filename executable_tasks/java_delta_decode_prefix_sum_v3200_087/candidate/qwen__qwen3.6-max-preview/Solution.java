public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0;
    long sumValues = 0;
    long w = deltas.length;
    for (int d : deltas) {
      sumValues += w * d;
      w--;
    }
    return sumValues + (long)deltas.length * 0x9E3779B1L;
  }
}