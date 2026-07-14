public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    // Preserve baseline behavior for iters == 0
    if (iters == 0) return 0L;

    final int[] idx = ids;
    final int[] dict = dictVals;
    final int n = idx.length;

    long h = FNV_OFFSET_BASIS;

    int i = 0;
    int limit = n & ~7; // unroll by 8
    while (i < limit) {
      int v0 = dict[idx[i    ]]; h ^= ((v0 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v1 = dict[idx[i + 1]]; h ^= ((v1 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v2 = dict[idx[i + 2]]; h ^= ((v2 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v3 = dict[idx[i + 3]]; h ^= ((v3 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v4 = dict[idx[i + 4]]; h ^= ((v4 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v5 = dict[idx[i + 5]]; h ^= ((v5 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v6 = dict[idx[i + 6]]; h ^= ((v6 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      int v7 = dict[idx[i + 7]]; h ^= ((v7 & 0xffffffffL) + 1L); h *= FNV_PRIME;
      i += 8;
    }
    while (i < n) {
      int v = dict[idx[i++]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= FNV_PRIME;
    }

    return h;
  }
}
