public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters == 0) return 0L;

    final int[] lIds = ids;
    final int[] lDict = dictVals;
    final int n = lIds.length;

    long h = 1469598103934665603L;
    for (int i = 0; i < n; i++) {
      int idx = lIds[i];
      int v = lDict[idx];
      h ^= (v & 0xffffffffL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }
}
