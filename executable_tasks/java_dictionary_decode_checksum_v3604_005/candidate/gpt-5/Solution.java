public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    // If no iterations requested, original code returns 0
    if (iters <= 0) return 0L;

    final int[] idsLocal = ids;
    final int[] dictLocal = dictVals;
    final int n = idsLocal.length;
    final long prime = 1099511628211L;
    long h = 1469598103934665603L;

    int i = 0;
    int end = n - (n & 3); // process in chunks of 4
    while (i < end) {
      int id0 = idsLocal[i];
      int id1 = idsLocal[i + 1];
      int id2 = idsLocal[i + 2];
      int id3 = idsLocal[i + 3];

      int v0 = dictLocal[id0];
      int v1 = dictLocal[id1];
      int v2 = dictLocal[id2];
      int v3 = dictLocal[id3];

      h ^= (((long) v0 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= (((long) v1 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= (((long) v2 & 0xffffffffL) + 1L);
      h *= prime;
      h ^= (((long) v3 & 0xffffffffL) + 1L);
      h *= prime;

      i += 4;
    }

    for (; i < n; i++) {
      int v = dictLocal[idsLocal[i]];
      h ^= (((long) v & 0xffffffffL) + 1L);
      h *= prime;
    }

    return h;
  }
}
