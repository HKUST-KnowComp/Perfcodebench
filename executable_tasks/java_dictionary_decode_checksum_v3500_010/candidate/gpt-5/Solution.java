public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0L;

    final int[] ii = ids;
    final int[] dv = dictVals;
    final int n = ii.length;
    long h = 1469598103934665603L;
    final long prime = 1099511628211L;

    int i = 0;
    int limit = n & ~7;
    for (; i < limit; i += 8) {
      int v0 = dv[ii[i]];
      h = (h ^ ((v0 & 0xffffffffL) + 1L)) * prime;
      int v1 = dv[ii[i + 1]];
      h = (h ^ ((v1 & 0xffffffffL) + 1L)) * prime;
      int v2 = dv[ii[i + 2]];
      h = (h ^ ((v2 & 0xffffffffL) + 1L)) * prime;
      int v3 = dv[ii[i + 3]];
      h = (h ^ ((v3 & 0xffffffffL) + 1L)) * prime;
      int v4 = dv[ii[i + 4]];
      h = (h ^ ((v4 & 0xffffffffL) + 1L)) * prime;
      int v5 = dv[ii[i + 5]];
      h = (h ^ ((v5 & 0xffffffffL) + 1L)) * prime;
      int v6 = dv[ii[i + 6]];
      h = (h ^ ((v6 & 0xffffffffL) + 1L)) * prime;
      int v7 = dv[ii[i + 7]];
      h = (h ^ ((v7 & 0xffffffffL) + 1L)) * prime;
    }
    for (; i < n; i++) {
      int v = dv[ii[i]];
      h = (h ^ ((v & 0xffffffffL) + 1L)) * prime;
    }

    return h;
  }
}
