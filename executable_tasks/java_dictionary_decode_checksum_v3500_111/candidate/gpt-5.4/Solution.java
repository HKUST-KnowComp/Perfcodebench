public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int len = localIds.length;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = 1469598103934665603L;

      int i = 0;
      int limit = len - 7;
      while (i < limit) {
        int v0 = localDict[localIds[i]];
        hash ^= (v0 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v1 = localDict[localIds[i + 1]];
        hash ^= (v1 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v2 = localDict[localIds[i + 2]];
        hash ^= (v2 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v3 = localDict[localIds[i + 3]];
        hash ^= (v3 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v4 = localDict[localIds[i + 4]];
        hash ^= (v4 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v5 = localDict[localIds[i + 5]];
        hash ^= (v5 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v6 = localDict[localIds[i + 6]];
        hash ^= (v6 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        int v7 = localDict[localIds[i + 7]];
        hash ^= (v7 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        i += 8;
      }

      while (i < len) {
        int v = localDict[localIds[i++]];
        hash ^= (v & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
      }

      h = hash;
    }
    return h;
  }
}
