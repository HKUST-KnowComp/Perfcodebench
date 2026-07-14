public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0 || deltas.length == 0) {
      return 0L;
    }
    int n = deltas.length;
    // Sum of prefix values: sum_{i=0}^{n-1} prefix[i]
    // where prefix[i] = delta[0] + delta[1] + ... + delta[i]
    // This equals sum_{i=0}^{n-1} delta[i] * (n - i)
    long prefixSum = 0L;
    for (int i = 0; i < n; i++) {
      prefixSum += (long) deltas[i] * (n - i);
    }
    // Each iteration adds 0x9E3779B1L to each of the n values
    long constantOffset = (long) n * 0x9E3779B1L;
    // The loop overwrites total each iteration, so only the last iteration's result matters
    // But wait - the baseline recomputes total each iteration and only returns the last one
    // So we just need the result of one iteration
    return prefixSum + constantOffset;
  }
}