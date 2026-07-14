public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long hash = 1469598103934665603L;

      int i = 0;
      int limit = len & ~7;
      while (i < limit) {
        int v0 = dictVals[ids[i]];
        hash ^= (v0 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v1 = dictVals[ids[i + 1]];
        hash ^= (v1 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v2 = dictVals[ids[i + 2]];
        hash ^= (v2 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v3 = dictVals[ids[i + 3]];
        hash ^= (v3 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v4 = dictVals[ids[i + 4]];
        hash ^= (v4 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v5 = dictVals[ids[i + 5]];
        hash ^= (v5 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v6 = dictVals[ids[i + 6]];
        hash ^= (v6 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v7 = dictVals[ids[i + 7]];
        hash ^= (v7 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        i += 8;
      }

      while (i < len) {
        int v = dictVals[ids[i++]];
        hash ^= (v & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
      }

      h = hash;
    }

    return h;
  }
}
