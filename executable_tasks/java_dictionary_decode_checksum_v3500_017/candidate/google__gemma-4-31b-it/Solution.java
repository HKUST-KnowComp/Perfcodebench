public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final long MULT = 1099511628211L;
    final long INIT = 1469598103934665603L;
    
    for (int iter = 0; iter < iters; iter++) {
      h = INIT;
      for (int i = 0; i < ids.length; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= MULT;
      }
    }
    return h;
  }
}