public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final long PRIME = 1099511628211L;
    final long OFFSET = 1469598103934665603L;

    // Reuse histogram across iterations; zeroed as part of checksum pass
    long[] hist = new long[buckets];

    final int[] kArr = keys;
    final int[] vArr = vals;
    final int n = kArr.length;

    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      // Accumulate values into histogram (unrolled)
      long[] hst = hist;
      int i = 0;
      int limit = n - 4;
      while (i <= limit) {
        int k0 = kArr[i];       hst[k0] += vArr[i];       i++;
        int k1 = kArr[i];       hst[k1] += vArr[i];       i++;
        int k2 = kArr[i];       hst[k2] += vArr[i];       i++;
        int k3 = kArr[i];       hst[k3] += vArr[i];       i++;
      }
      for (; i < n; i++) {
        hst[kArr[i]] += vArr[i];
      }

      // Compute checksum and clear histogram in one pass (unrolled)
      h = OFFSET;
      int m = buckets;
      int j = 0;
      int mlim = m - 4;
      while (j <= mlim) {
        long v0 = hst[j];
        h ^= (v0 + 1L);
        h *= PRIME;
        hst[j] = 0L;

        long v1 = hst[j + 1];
        h ^= (v1 + 1L);
        h *= PRIME;
        hst[j + 1] = 0L;

        long v2 = hst[j + 2];
        h ^= (v2 + 1L);
        h *= PRIME;
        hst[j + 2] = 0L;

        long v3 = hst[j + 3];
        h ^= (v3 + 1L);
        h *= PRIME;
        hst[j + 3] = 0L;

        j += 4;
      }
      for (; j < m; j++) {
        long v = hst[j];
        h ^= (v + 1L);
        h *= PRIME;
        hst[j] = 0L;
      }
    }

    return h;
  }
}
