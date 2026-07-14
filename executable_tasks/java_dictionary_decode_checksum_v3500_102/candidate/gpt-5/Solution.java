public class Solution {
  private static final long OFFSET = 1469598103934665603L;
  private static final long PRIME = 1099511628211L;

  public static long run(int[] ids, int[] dictVals, int iters) {
    final int n = ids.length;
    if (iters <= 0) return 0L;
    if (n == 0) return OFFSET;

    // Fast path for single iteration: avoid extra allocations and preprocessing
    if (iters == 1) {
      long h = OFFSET;
      for (int i = 0; i < n; i++) {
        int v = dictVals[ids[i]];
        h ^= ((long) v & 0xffffffffL) + 1L;
        h *= PRIME;
      }
      return h;
    }

    // Precompute once for multiple iterations
    final long[] elems = new long[n];
    for (int i = 0; i < n; i++) {
      elems[i] = ((long) dictVals[ids[i]] & 0xffffffffL) + 1L;
    }

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long x = OFFSET;
      int i = 0;
      int limit = n - (n & 7); // n - (n % 8)
      while (i < limit) {
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
        x ^= elems[i++];
        x *= PRIME;
      }
      while (i < n) {
        x ^= elems[i++];
        x *= PRIME;
      }
      h = x;
    }
    return h;
  }
}
