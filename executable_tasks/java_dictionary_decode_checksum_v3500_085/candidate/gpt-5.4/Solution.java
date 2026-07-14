public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int[] localIds = ids;
    final int[] localDict = dictVals;
    final int n = localIds.length;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int v = localDict[localIds[i]];
        hash ^= (v & 0xffffffffL) + 1L;
        hash *= 1099511628211L;
      }
      h = hash;
    }
    return h;
  }
}
