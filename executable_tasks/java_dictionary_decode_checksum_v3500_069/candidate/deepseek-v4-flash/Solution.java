public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long[] inc = new long[ids.length];
    for (int i = 0; i < ids.length; i++) {
      inc[i] = (dictVals[ids[i]] & 0xffffffffL) + 1L;
    }
    long h = 0L;
    final long prime = 1099511628211L;
    final long offset = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      h = offset;
      for (int i = 0; i < inc.length; i++) {
        h ^= inc[i];
        h *= prime;
      }
    }
    return h;
  }
}