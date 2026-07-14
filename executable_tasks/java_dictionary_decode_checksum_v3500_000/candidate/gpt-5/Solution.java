public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0L;

    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;

    long h = 1469598103934665603L; // FNV offset basis
    final long P = 1099511628211L;  // FNV prime

    int i = 0;
    // Unroll by 8 for throughput
    for (; i + 7 < n; i += 8) {
      int id0 = localIds[i];
      int id1 = localIds[i + 1];
      int id2 = localIds[i + 2];
      int id3 = localIds[i + 3];
      int id4 = localIds[i + 4];
      int id5 = localIds[i + 5];
      int id6 = localIds[i + 6];
      int id7 = localIds[i + 7];

      long x0 = (localDict[id0] & 0xffffffffL) + 1L;
      h ^= x0; h *= P;
      long x1 = (localDict[id1] & 0xffffffffL) + 1L;
      h ^= x1; h *= P;
      long x2 = (localDict[id2] & 0xffffffffL) + 1L;
      h ^= x2; h *= P;
      long x3 = (localDict[id3] & 0xffffffffL) + 1L;
      h ^= x3; h *= P;
      long x4 = (localDict[id4] & 0xffffffffL) + 1L;
      h ^= x4; h *= P;
      long x5 = (localDict[id5] & 0xffffffffL) + 1L;
      h ^= x5; h *= P;
      long x6 = (localDict[id6] & 0xffffffffL) + 1L;
      h ^= x6; h *= P;
      long x7 = (localDict[id7] & 0xffffffffL) + 1L;
      h ^= x7; h *= P;
    }

    for (; i < n; i++) {
      int id = localIds[i];
      long x = (localDict[id] & 0xffffffffL) + 1L;
      h ^= x;
      h *= P;
    }

    return h;
  }
}
