public class Solution {
  private static final long OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    // Baseline returns 0 when no iterations are performed.
    if (iters <= 0) return 0L;

    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;

    long h = OFFSET_BASIS;

    // Unroll the main loop to reduce overhead.
    int i = 0;
    int limit = n - (n & 3); // n - (n % 4)
    while (i < limit) {
      int id0 = localIds[i];
      int id1 = localIds[i + 1];
      int id2 = localIds[i + 2];
      int id3 = localIds[i + 3];

      int v0 = localDict[id0];
      h ^= (v0 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v1 = localDict[id1];
      h ^= (v1 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v2 = localDict[id2];
      h ^= (v2 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      int v3 = localDict[id3];
      h ^= (v3 & 0xffffffffL) + 1L;
      h *= FNV_PRIME;

      i += 4;
    }

    // Remainder
    while (i < n) {
      int v = localDict[localIds[i++]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= FNV_PRIME;
    }

    return h;
  }
}
