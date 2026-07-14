public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long current = 0;
    long sumValues = 0;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      sumValues += current;
    }
    long constPart = (long) n * 0x9E3779B1L;
    return iters * (sumValues + constPart);
  }
}