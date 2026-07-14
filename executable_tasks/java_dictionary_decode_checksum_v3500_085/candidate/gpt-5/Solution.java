public class Solution {
  private static final long OFFSET = 1469598103934665603L;
  private static final long PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0L;
    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;

    for (int iter = 0; iter < iters; iter++) {
      long hh = OFFSET;
      int i = 0;

      int limit = n & ~7; // unroll by 8
      while (i < limit) {
        int id0 = localIds[i];
        int id1 = localIds[i + 1];
        int id2 = localIds[i + 2];
        int id3 = localIds[i + 3];
        int id4 = localIds[i + 4];
        int id5 = localIds[i + 5];
        int id6 = localIds[i + 6];
        int id7 = localIds[i + 7];

        int v0 = localDict[id0];
        int v1 = localDict[id1];
        int v2 = localDict[id2];
        int v3 = localDict[id3];
        int v4 = localDict[id4];
        int v5 = localDict[id5];
        int v6 = localDict[id6];
        int v7 = localDict[id7];

        hh ^= ((v0 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v1 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v2 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v3 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v4 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v5 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v6 & 0xffffffffL) + 1L);
        hh *= PRIME;
        hh ^= ((v7 & 0xffffffffL) + 1L);
        hh *= PRIME;

        i += 8;
      }

      while (i < n) {
        int v = localDict[localIds[i++]];
        hh ^= ((v & 0xffffffffL) + 1L);
        hh *= PRIME;
      }

      h = hh;
    }

    return h;
  }
}
