import java.util.Arrays;

public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    long[] hist = new long[buckets];
    final long FNV_PRIME = 1099511628211L;
    final long FNV_OFFSET = 1469598103934665603L;
    int n = keys.length;

    for (int iter = 0; iter < iters; iter++) {
      Arrays.fill(hist, 0L);
      for (int i = 0; i < n; i++) {
        hist[keys[i]] += vals[i];
      }
      h = FNV_OFFSET;
      for (int i = 0; i < buckets; i++) {
        h ^= (hist[i] + 1);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}