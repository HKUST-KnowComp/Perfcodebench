public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    for (int iter = 0; iter < iters; iter++) {
      h = FNV_OFFSET;
      for (int id : ids) {
        int v = dictVals[id];
        long val = (v & 0xffffffffL) + 1L;
        h ^= val;
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}