public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    // Precompute decoded values once, avoid redundant work per iteration
    int[] decoded = new int[ids.length];
    for (int i = 0; i < ids.length; i++) {
      decoded[i] = dictVals[ids[i]];
    }
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (int v : decoded) {
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}