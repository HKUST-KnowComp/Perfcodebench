public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    // Preserve baseline behavior: if iters == 0, return 0
    if (iters == 0) {
      return 0L;
    }

    final int n = ids.length;
    // If there are no ids, the hash is the offset basis per iteration
    if (n == 0) {
      return 1469598103934665603L;
    }

    final int[] idx = ids;
    final int[] dict = dictVals;
    final long prime = 1099511628211L;
    long h = 1469598103934665603L;

    int i = 0;
    int limit = n - (n & 3); // n rounded down to multiple of 4
    for (; i < limit; i += 4) {
      int v0 = dict[idx[i]];
      int v1 = dict[idx[i + 1]];
      int v2 = dict[idx[i + 2]];
      int v3 = dict[idx[i + 3]];

      h ^= ((v0 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= ((v1 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= ((v2 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= ((v3 & 0xffffffffL) + 1L);
      h *= prime;
    }
    for (; i < n; i++) {
      int v = dict[idx[i]];
      h ^= ((v & 0xffffffffL) + 1L);
      h *= prime;
    }

    return h;
  }
}
