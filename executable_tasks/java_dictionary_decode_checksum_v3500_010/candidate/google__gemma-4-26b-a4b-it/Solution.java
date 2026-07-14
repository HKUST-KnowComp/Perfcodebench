public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      // We must reset h to the initial FNV offset for every iteration to maintain correctness
      // based on the original logic where h is reset inside the iter loop.
      long currentH = 1469598103934665603L;
      for (int i = 0; i < ids.length; i++) {
        int v = dictVals[ids[i]];
        currentH ^= (v & 0xffffffffL) + 1L;
        currentH *= 1099511628211L;
      }
      h = currentH;
    }
    return h;
  }
}