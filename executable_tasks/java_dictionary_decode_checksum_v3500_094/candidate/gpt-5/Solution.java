public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    // Baseline returns 0 when iters == 0 (h initialized to 0 and never changed)
    if (iters <= 0) return 0L;

    final int[] idsLocal = ids;
    final int[] dict = dictVals;
    final int n = idsLocal.length;
    long h = 1469598103934665603L; // FNV offset basis
    final long prime = 1099511628211L; // FNV prime

    int i = 0;
    int limit = n - 4;
    // Unrolled processing by 4 for reduced loop overhead
    for (; i <= limit; i += 4) {
      int v0 = dict[idsLocal[i]];
      h ^= (((long) v0 & 0xffffffffL) + 1L);
      h *= prime;

      int v1 = dict[idsLocal[i + 1]];
      h ^= (((long) v1 & 0xffffffffL) + 1L);
      h *= prime;

      int v2 = dict[idsLocal[i + 2]];
      h ^= (((long) v2 & 0xffffffffL) + 1L);
      h *= prime;

      int v3 = dict[idsLocal[i + 3]];
      h ^= (((long) v3 & 0xffffffffL) + 1L);
      h *= prime;
    }
    // Tail processing
    for (; i < n; i++) {
      int v = dict[idsLocal[i]];
      h ^= (((long) v & 0xffffffffL) + 1L);
      h *= prime;
    }

    return h;
  }
}
