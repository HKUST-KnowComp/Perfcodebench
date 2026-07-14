public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    int[] decoded = new int[ids.length];
    for (int i = 0; i < ids.length; i++) {
      decoded[i] = dictVals[ids[i]];
    }
    
    long h = 1469598103934665603L;
    for (int iter = 0; iter < iters; iter++) {
      for (int v : decoded) {
        h ^= (v & 0xffffffffL) + 1L;
        h *= 1099511628211L;
      }
    }
    return h;
  }
}