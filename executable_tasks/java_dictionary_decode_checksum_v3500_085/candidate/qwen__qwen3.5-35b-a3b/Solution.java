public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 1469598103934665603L;
    for (int id : ids) {
      int v = dictVals[id];
      h ^= (v & 0xffffffffL) + 1L;
      h *= 1099511628211L;
    }
    return h;
  }
}