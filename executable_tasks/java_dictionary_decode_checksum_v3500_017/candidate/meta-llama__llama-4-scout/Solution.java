public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    int len = ids.length;
    int[] decoded = new int[len];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i++) {
        decoded[i] = dictVals[ids[i]];
      }
      h = 1469598103934665603L;
      for (int v : decoded) {
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}