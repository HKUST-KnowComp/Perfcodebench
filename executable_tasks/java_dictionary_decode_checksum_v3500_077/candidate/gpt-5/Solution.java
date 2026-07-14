public class Solution {
  private static final long OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    // Baseline returns 0 when iters == 0
    if (iters == 0) return 0L;

    final int[] idsLocal = ids;
    final int[] dict = dictVals;
    final int n = idsLocal.length;

    long h = OFFSET_BASIS;

    int i = 0;
    int limit = n - (n & 3); // process in chunks of 4
    for (; i < limit; i += 4) {
      int v0 = dict[idsLocal[i]];
      h ^= (v0 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v1 = dict[idsLocal[i + 1]];
      h ^= (v1 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v2 = dict[idsLocal[i + 2]];
      h ^= (v2 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v3 = dict[idsLocal[i + 3]];
      h ^= (v3 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;
    }

    // Remainder
    for (; i < n; i++) {
      int v = dict[idsLocal[i]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= FNV_PRIME;
    }

    return h;
  }
}
