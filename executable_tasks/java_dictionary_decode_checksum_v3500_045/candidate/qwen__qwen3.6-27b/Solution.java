public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters <= 0) return 0;
    long h = 1469598103934665603L;
    int len = ids.length;
    for (int i = 0; i < len; i++) {
      int v = dictVals[ids[i]];
      h ^= (v & 0xffffffffL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }
}