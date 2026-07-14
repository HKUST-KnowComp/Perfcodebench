public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    int len = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      // We must reset h to the initial seed for every iteration to match the baseline behavior
      // However, the baseline resets h inside the loop, so we follow that logic.
      h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        long v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}