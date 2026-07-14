public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters == 0) return 0L;
    long h = 1469598103934665603L;
    final long mult = 1099511628211L;
    for (int id : ids) {
      int v = dictVals[id];
      h ^= (v & 0xffffffffL) + 1L;
      h *= mult;
    }
    return h;
  }
}