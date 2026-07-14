public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int len = localIds.length;
    final long offset = 1469598103934665603L;
    final long prime = 1099511628211L;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = offset;
      int i = 0;
      int limit = len - 3;
      while (i < limit) {
        int v0 = localDict[localIds[i]];
        hash ^= (v0 & 0xffffffffL) + 1L;
        hash *= prime;

        int v1 = localDict[localIds[i + 1]];
        hash ^= (v1 & 0xffffffffL) + 1L;
        hash *= prime;

        int v2 = localDict[localIds[i + 2]];
        hash ^= (v2 & 0xffffffffL) + 1L;
        hash *= prime;

        int v3 = localDict[localIds[i + 3]];
        hash ^= (v3 & 0xffffffffL) + 1L;
        hash *= prime;

        i += 4;
      }
      while (i < len) {
        int v = localDict[localIds[i++]];
        hash ^= (v & 0xffffffffL) + 1L;
        hash *= prime;
      }
      h = hash;
    }
    return h;
  }
}
