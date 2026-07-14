public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final long PRIME = 1099511628211L;
    final long OFFSET = 1469598103934665603L;

    final long[] hist = new long[buckets];
    final int[] k = keys;
    final int[] v = vals;
    final int n = k.length;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      // Accumulate into histogram
      int i = 0;
      int limit = n - 4;
      for (; i <= limit; i += 4) {
        int k0 = k[i];
        hist[k0] += v[i];
        int k1 = k[i + 1];
        hist[k1] += v[i + 1];
        int k2 = k[i + 2];
        hist[k2] += v[i + 2];
        int k3 = k[i + 3];
        hist[k3] += v[i + 3];
      }
      for (; i < n; i++) {
        hist[k[i]] += v[i];
      }

      // Compute checksum and clear histogram in the same pass
      h = OFFSET;
      int b = buckets;
      int j = 0;
      int jlimit = b - 4;
      for (; j <= jlimit; j += 4) {
        long v0 = hist[j];
        h ^= (v0 + 1L);
        h *= PRIME;
        hist[j] = 0L;

        long v1v = hist[j + 1];
        h ^= (v1v + 1L);
        h *= PRIME;
        hist[j + 1] = 0L;

        long v2v = hist[j + 2];
        h ^= (v2v + 1L);
        h *= PRIME;
        hist[j + 2] = 0L;

        long v3v = hist[j + 3];
        h ^= (v3v + 1L);
        h *= PRIME;
        hist[j + 3] = 0L;
      }
      for (; j < b; j++) {
        long vv = hist[j];
        h ^= (vv + 1L);
        h *= PRIME;
        hist[j] = 0L;
      }
    }
    return h;
  }
}
