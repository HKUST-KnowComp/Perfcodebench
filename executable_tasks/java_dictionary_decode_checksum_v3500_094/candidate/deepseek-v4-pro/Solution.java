public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    if (iters == 0) return 0L;
    int n = ids.length;
    long[] vals = new long[n];
    for (int i = 0; i < n; i++) {
      int v = dictVals[ids[i]];
      vals[i] = (v & 0xffffffffL) + 1L;
    }
    final long M = 1099511628211L;
    final long INIT = 1469598103934665603L;
    long result = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long h = INIT;
      int i = 0;
      int limit = n - (n % 4);
      for (; i < limit; i += 4) {
        h ^= vals[i];
        h *= M;
        h ^= vals[i + 1];
        h *= M;
        h ^= vals[i + 2];
        h *= M;
        h ^= vals[i + 3];
        h *= M;
      }
      for (; i < n; i++) {
        h ^= vals[i];
        h *= M;
      }
      result = h;
    }
    return result;
  }
}