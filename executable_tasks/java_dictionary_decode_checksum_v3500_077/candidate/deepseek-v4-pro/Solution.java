public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) {
      return 0L;
    }
    long h = 1469598103934665603L;
    int[] localIds = ids;
    int[] localDict = dictVals;
    int len = localIds.length;
    for (int i = 0; i < len; i++) {
      int v = localDict[localIds[i]];
      h ^= (v & 0xFFFFFFFFL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }
}