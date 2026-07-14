public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int len = keys.length;
    final long[] hist = new long[buckets];
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      // Accumulate values into histogram
      final long[] localHist = hist;
      final int[] kArr = keys;
      final int[] vArr = vals;
      int i = 0;
      int n = len;
      int limit = n & ~7; // process 8 at a time
      for (; i < limit; i += 8) {
        int k0 = kArr[i];     localHist[k0] += vArr[i];
        int k1 = kArr[i + 1]; localHist[k1] += vArr[i + 1];
        int k2 = kArr[i + 2]; localHist[k2] += vArr[i + 2];
        int k3 = kArr[i + 3]; localHist[k3] += vArr[i + 3];
        int k4 = kArr[i + 4]; localHist[k4] += vArr[i + 4];
        int k5 = kArr[i + 5]; localHist[k5] += vArr[i + 5];
        int k6 = kArr[i + 6]; localHist[k6] += vArr[i + 6];
        int k7 = kArr[i + 7]; localHist[k7] += vArr[i + 7];
      }
      for (; i < n; i++) {
        localHist[kArr[i]] += vArr[i];
      }

      // Compute checksum and zero histogram in the same pass
      long hh = FNV_OFFSET;
      int b = buckets;
      int j = 0;
      int blimit = b & ~3; // unroll by 4
      for (; j < blimit; j += 4) {
        long v0 = localHist[j];
        long v1 = localHist[j + 1];
        long v2 = localHist[j + 2];
        long v3 = localHist[j + 3];

        hh ^= (v0 + 1); hh *= FNV_PRIME;
        hh ^= (v1 + 1); hh *= FNV_PRIME;
        hh ^= (v2 + 1); hh *= FNV_PRIME;
        hh ^= (v3 + 1); hh *= FNV_PRIME;

        localHist[j] = 0L;
        localHist[j + 1] = 0L;
        localHist[j + 2] = 0L;
        localHist[j + 3] = 0L;
      }
      for (; j < b; j++) {
        long v = localHist[j];
        hh ^= (v + 1);
        hh *= FNV_PRIME;
        localHist[j] = 0L;
      }
      h = hh;
    }
    return h;
  }
}