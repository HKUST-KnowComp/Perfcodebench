public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0L;

    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;
    final long PRIME = 1099511628211L;

    long h = 1469598103934665603L;

    int i = 0;
    int limit = n - 4;
    for (; i <= limit; i += 4) {
      int v0 = localDict[localIds[i]];
      h ^= (((long) v0) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v1 = localDict[localIds[i + 1]];
      h ^= (((long) v1) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v2 = localDict[localIds[i + 2]];
      h ^= (((long) v2) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v3 = localDict[localIds[i + 3]];
      h ^= (((long) v3) & 0xffffffffL) + 1L;
      h *= PRIME;
    }
    for (; i < n; i++) {
      int v = localDict[localIds[i]];
      h ^= (((long) v) & 0xffffffffL) + 1L;
      h *= PRIME;
    }

    return h;
  }
}
