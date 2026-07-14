public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters == 0) {
      return 0L;
    }
    long h = 1469598103934665603L;
    for (int id : ids) {
      long v = (dictVals[id] & 0xffffffffL) + 1L;
      h ^= v;
      h *= 1099511628211L;
    }
    return h;
  }
}