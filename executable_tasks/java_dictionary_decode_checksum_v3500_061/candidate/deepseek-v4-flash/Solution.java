public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    int n = ids.length;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}