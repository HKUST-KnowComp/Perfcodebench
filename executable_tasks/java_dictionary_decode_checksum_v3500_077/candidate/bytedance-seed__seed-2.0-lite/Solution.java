public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final int idsLen = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < idsLen; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xFFFFFFFFL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}