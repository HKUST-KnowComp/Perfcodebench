public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    final int[] decoded = new int[len];
    for (int i = 0; i < len; i++) {
      decoded[i] = dictVals[ids[i]];
    }
    final long init = 1469598103934665603L;
    final long mul = 1099511628211L;
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      h = init;
      int i = 0;
      for (; i + 3 < len; i += 4) {
        long v0 = decoded[i] & 0xffffffffL;
        long v1 = decoded[i+1] & 0xffffffffL;
        long v2 = decoded[i+2] & 0xffffffffL;
        long v3 = decoded[i+3] & 0xffffffffL;
        h ^= v0 + 1L; h *= mul;
        h ^= v1 + 1L; h *= mul;
        h ^= v2 + 1L; h *= mul;
        h ^= v3 + 1L; h *= mul;
      }
      for (; i < len; i++) {
        long v = decoded[i] & 0xffffffffL;
        h ^= v + 1L;
        h *= mul;
      }
    }
    return h;
  }
}