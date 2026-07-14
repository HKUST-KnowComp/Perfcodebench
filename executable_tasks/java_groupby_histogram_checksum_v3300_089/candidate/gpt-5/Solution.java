public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    if (iters <= 0) return 0L;

    long[] hist = new long[buckets];

    int n = keys.length;
    int[] k = keys;
    int[] v = vals;
    long[] hst = hist;

    int i = 0;
    int limit = n & ~3; // unroll by 4
    while (i < limit) {
      int k0 = k[i];
      int k1 = k[i + 1];
      int k2 = k[i + 2];
      int k3 = k[i + 3];
      hst[k0] += v[i];
      hst[k1] += v[i + 1];
      hst[k2] += v[i + 2];
      hst[k3] += v[i + 3];
      i += 4;
    }
    for (; i < n; i++) {
      hst[k[i]] += v[i];
    }

    long h = 1469598103934665603L; // FNV-1a 64-bit offset
    for (int b = 0; b < buckets; b++) {
      long vv = hst[b] + 1L;
      h ^= vv;
      h *= 1099511628211L; // FNV-1a 64-bit prime
    }

    return h;
  }
}