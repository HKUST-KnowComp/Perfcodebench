public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME  = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    final int len = ids.length;
    if (iters == 0) return 0L;

    int[] decoded = new int[len];
    long h = FNV_OFFSET;

    // decode and compute first hash in one pass
    for (int i = 0; i < len; i++) {
      int v = dictVals[ids[i]];
      decoded[i] = v;
      h ^= (v & 0xffffffffL) + 1L;
      h *= FNV_PRIME;
    }

    // remaining iterations: only hash over decoded array
    for (int iter = 1; iter < iters; iter++) {
      h = FNV_OFFSET;
      for (int i = 0; i < len; i++) {
        h ^= (decoded[i] & 0xffffffffL) + 1L;
        h *= FNV_PRIME;
      }
    }

    return h;
  }
}