public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final long seed = 1469598103934665603L;
    final long multiplier = 1099511628211L;
    
    for (int iter = 0; iter < iters; iter++) {
      h = seed;
      for (int i = 0; i < ids.length; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= multiplier;
      }
    }
    return h;
  }
}