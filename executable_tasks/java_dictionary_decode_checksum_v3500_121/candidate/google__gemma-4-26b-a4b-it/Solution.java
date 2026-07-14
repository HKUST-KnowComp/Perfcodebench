public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    int len = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      // Reset hash for each iteration as per original logic
      // Note: The original code resets h inside the loop, but the final return is the last h.
      // To match the exact behavior: the loop calculates h based on the current iteration's decoded values.
      h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}