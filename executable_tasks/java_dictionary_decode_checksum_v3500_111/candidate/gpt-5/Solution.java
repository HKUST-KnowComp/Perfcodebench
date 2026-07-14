public class Solution {
  private static final long OFFSET = 1469598103934665603L;
  private static final long PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0L;

    int[] idsLocal = ids;
    int[] dict = dictVals;
    int n = idsLocal.length;

    long h = OFFSET;

    int i = 0;
    int unrollLimit = n & ~3; // largest multiple of 4 <= n
    while (i < unrollLimit) {
      int v0 = dict[idsLocal[i]];
      h ^= (v0 & 0xffffffffL) + 1L;
      h *= PRIME;

      int v1 = dict[idsLocal[i + 1]];
      h ^= (v1 & 0xffffffffL) + 1L;
      h *= PRIME;

      int v2 = dict[idsLocal[i + 2]];
      h ^= (v2 & 0xffffffffL) + 1L;
      h *= PRIME;

      int v3 = dict[idsLocal[i + 3]];
      h ^= (v3 & 0xffffffffL) + 1L;
      h *= PRIME;

      i += 4;
    }
    for (; i < n; i++) {
      int v = dict[idsLocal[i]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= PRIME;
    }

    return h;
  }
}