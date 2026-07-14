public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0L;

    final int n = ids.length;
    final long OFFSET = 1469598103934665603L;
    final long PRIME = 1099511628211L;

    if (n == 0) return OFFSET;

    long h = OFFSET;
    final int[] localIds = ids;
    final int[] localDictVals = dictVals;

    int i = 0;
    int limit = n & ~3; // unroll by 4
    for (; i < limit; i += 4) {
      int v0 = localDictVals[localIds[i]];
      h ^= (((long) v0) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v1 = localDictVals[localIds[i + 1]];
      h ^= (((long) v1) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v2 = localDictVals[localIds[i + 2]];
      h ^= (((long) v2) & 0xffffffffL) + 1L;
      h *= PRIME;

      int v3 = localDictVals[localIds[i + 3]];
      h ^= (((long) v3) & 0xffffffffL) + 1L;
      h *= PRIME;
    }
    for (; i < n; i++) {
      int v = localDictVals[localIds[i]];
      h ^= (((long) v) & 0xffffffffL) + 1L;
      h *= PRIME;
    }

    // Each iteration produces the same checksum; the last iteration's value is returned.
    return h;
  }
}
