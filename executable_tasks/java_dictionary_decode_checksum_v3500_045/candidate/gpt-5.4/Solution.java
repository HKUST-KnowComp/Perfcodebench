public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = 1469598103934665603L;

      int i = 0;
      int limit = n & ~7;
      while (i < limit) {
        int v0 = localDict[localIds[i]];
        int v1 = localDict[localIds[i + 1]];
        int v2 = localDict[localIds[i + 2]];
        int v3 = localDict[localIds[i + 3]];
        int v4 = localDict[localIds[i + 4]];
        int v5 = localDict[localIds[i + 5]];
        int v6 = localDict[localIds[i + 6]];
        int v7 = localDict[localIds[i + 7]];

        hash ^= (v0 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v1 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v2 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v3 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v4 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v5 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v6 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
        hash ^= (v7 & 0xffffffffL) + 1L;
        hash *= 1099511628211L;

        i += 8;
      }

      while (i < n) {
        int v = localDict[localIds[i++]];
        hash ^= (v & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
      }

      h = hash;
    }
    return h;
  }
}
