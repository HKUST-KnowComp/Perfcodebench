public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      long hh = 1469598103934665603L;
      int i = 0;
      // unroll 4x
      for (; i + 3 < len; i += 4) {
        long v0 = dictVals[ids[i]] & 0xffffffffL;
        long v1 = dictVals[ids[i + 1]] & 0xffffffffL;
        long v2 = dictVals[ids[i + 2]] & 0xffffffffL;
        long v3 = dictVals[ids[i + 3]] & 0xffffffffL;
        hh ^= (v0 + 1L);
        hh *= 1099511628211L;
        hh ^= (v1 + 1L);
        hh *= 1099511628211L;
        hh ^= (v2 + 1L);
        hh *= 1099511628211L;
        hh ^= (v3 + 1L);
        hh *= 1099511628211L;
      }
      // tail
      for (; i < len; i++) {
        hh ^= ((dictVals[ids[i]] & 0xffffffffL) + 1L);
        hh *= 1099511628211L;
      }
      h = hh;
    }
    return h;
  }
}