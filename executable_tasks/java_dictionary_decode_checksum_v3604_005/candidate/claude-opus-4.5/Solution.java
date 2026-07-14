public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    final int len4 = len & ~3;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      int i = 0;
      for (; i < len4; i += 4) {
        long v0 = dictVals[ids[i]] & 0xffffffffL;
        long v1 = dictVals[ids[i + 1]] & 0xffffffffL;
        long v2 = dictVals[ids[i + 2]] & 0xffffffffL;
        long v3 = dictVals[ids[i + 3]] & 0xffffffffL;
        h ^= v0 + 1L;
        h *= 1099511628211L;
        h ^= v1 + 1L;
        h *= 1099511628211L;
        h ^= v2 + 1L;
        h *= 1099511628211L;
        h ^= v3 + 1L;
        h *= 1099511628211L;
      }
      for (; i < len; i++) {
        h ^= (dictVals[ids[i]] & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}