public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    int len = keys.length;
    int[] k = keys;
    int[] v = vals;
    long[] hist = new long[buckets];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i++) {
        hist[k[i]] += v[i];
      }
      long hash = FNV_OFFSET;
      for (int i = 0; i < buckets; i++) {
        long bucketVal = hist[i];
        hash ^= (bucketVal + 1);
        hash *= FNV_PRIME;
        hist[i] = 0;
      }
      h = hash;
    }
    return h;
  }
}