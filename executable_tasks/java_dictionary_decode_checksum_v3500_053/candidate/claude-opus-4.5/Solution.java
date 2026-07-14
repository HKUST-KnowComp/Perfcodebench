public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        h ^= (dictVals[ids[i]] & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}