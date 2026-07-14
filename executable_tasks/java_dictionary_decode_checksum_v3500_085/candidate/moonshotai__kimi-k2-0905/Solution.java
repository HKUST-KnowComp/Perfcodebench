public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    final long seed = 1469598103934665603L;
    final long prime = 1099511628211L;

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = seed;
      int i = 0;
      // peel tail to allow unaligned vectorised main loop
      for (; i + 3 < len; i += 4) {
        int v0 = dictVals[ids[i]];
        int v1 = dictVals[ids[i + 1]];
        int v2 = dictVals[ids[i + 2]];
        int v3 = dictVals[ids[i + 3]];

        h ^= (v0 & 0xffffffffL) + 1L;  h *= prime;
        h ^= (v1 & 0xffffffffL) + 1L;  h *= prime;
        h ^= (v2 & 0xffffffffL) + 1L;  h *= prime;
        h ^= (v3 & 0xffffffffL) + 1L;  h *= prime;
      }
      // handle remaining 0-3 elements
      for (; i < len; i++) {
        h ^= (dictVals[ids[i]] & 0xffffffffL) + 1L;
        h *= prime;
      }
    }
    return h;
  }
}