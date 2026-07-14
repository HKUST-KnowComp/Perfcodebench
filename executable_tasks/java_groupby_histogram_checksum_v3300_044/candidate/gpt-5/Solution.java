public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final long[] hist = new long[buckets];
    final int[] lk = keys;
    final int[] lv = vals;
    final int klen = lk.length;
    final int b = buckets;
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int i = 0;
      int limit = klen & ~3; // unroll by 4
      for (; i < limit; i += 4) {
        hist[lk[i]] += lv[i];
        hist[lk[i + 1]] += lv[i + 1];
        hist[lk[i + 2]] += lv[i + 2];
        hist[lk[i + 3]] += lv[i + 3];
      }
      for (; i < klen; i++) {
        hist[lk[i]] += lv[i];
      }

      h = FNV64_OFFSET;
      int j = 0;
      int blimit = b & ~3; // unroll by 4
      for (; j < blimit; j += 4) {
        long v0 = hist[j] + 1L;
        long v1 = hist[j + 1] + 1L;
        long v2 = hist[j + 2] + 1L;
        long v3 = hist[j + 3] + 1L;

        h ^= v0;
        h *= FNV64_PRIME;
        h ^= v1;
        h *= FNV64_PRIME;
        h ^= v2;
        h *= FNV64_PRIME;
        h ^= v3;
        h *= FNV64_PRIME;

        hist[j] = 0L;
        hist[j + 1] = 0L;
        hist[j + 2] = 0L;
        hist[j + 3] = 0L;
      }
      for (; j < b; j++) {
        long v = hist[j] + 1L;
        h ^= v;
        h *= FNV64_PRIME;
        hist[j] = 0L;
      }
    }

    return h;
  }
}
