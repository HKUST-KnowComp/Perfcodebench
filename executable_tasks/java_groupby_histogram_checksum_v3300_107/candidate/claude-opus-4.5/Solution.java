public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    final int len = keys.length;
    final int lenMain = len - (len & 3);
    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];
      int i = 0;
      for (; i < lenMain; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i + 1]] += vals[i + 1];
        hist[keys[i + 2]] += vals[i + 2];
        hist[keys[i + 3]] += vals[i + 3];
      }
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }
      h = 1469598103934665603L;
      int b = 0;
      final int bucketsMain = buckets - (buckets & 3);
      for (; b < bucketsMain; b += 4) {
        h ^= (hist[b] + 1);
        h *= 1099511628211L;
        h ^= (hist[b + 1] + 1);
        h *= 1099511628211L;
        h ^= (hist[b + 2] + 1);
        h *= 1099511628211L;
        h ^= (hist[b + 3] + 1);
        h *= 1099511628211L;
      }
      for (; b < buckets; b++) {
        h ^= (hist[b] + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}