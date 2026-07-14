public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final long FNV_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    for (int iter = 0; iter < iters; iter++) {
      h = FNV_BASIS;
      for (int i = 0; i < ids.length; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}