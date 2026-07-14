public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0L;
    if (iters <= 0) return 0L;

    final long[] hist = new long[buckets];
    final int[] kArr = keys;
    final int[] vArr = vals;
    final int n = kArr.length;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      // Accumulate values into histogram
      int i = 0;
      int limit = n - 3;
      for (; i <= limit; i += 4) {
        int k0 = kArr[i];
        int k1 = kArr[i + 1];
        int k2 = kArr[i + 2];
        int k3 = kArr[i + 3];
        hist[k0] += vArr[i];
        hist[k1] += vArr[i + 1];
        hist[k2] += vArr[i + 2];
        hist[k3] += vArr[i + 3];
      }
      for (; i < n; i++) {
        hist[kArr[i]] += vArr[i];
      }

      // Compute checksum and clear histogram for next iteration
      long hh = FNV_OFFSET;
      int b = 0;
      int bLimit = buckets - 3;
      for (; b <= bLimit; b += 4) {
        long v0 = hist[b];
        long v1 = hist[b + 1];
        long v2 = hist[b + 2];
        long v3 = hist[b + 3];

        hh ^= (v0 + 1);
        hh *= FNV_PRIME;
        hh ^= (v1 + 1);
        hh *= FNV_PRIME;
        hh ^= (v2 + 1);
        hh *= FNV_PRIME;
        hh ^= (v3 + 1);
        hh *= FNV_PRIME;

        hist[b] = 0L;
        hist[b + 1] = 0L;
        hist[b + 2] = 0L;
        hist[b + 3] = 0L;
      }
      for (; b < buckets; b++) {
        long v = hist[b];
        hh ^= (v + 1);
        hh *= FNV_PRIME;
        hist[b] = 0L;
      }

      h = hh;
    }

    return h;
  }
}
