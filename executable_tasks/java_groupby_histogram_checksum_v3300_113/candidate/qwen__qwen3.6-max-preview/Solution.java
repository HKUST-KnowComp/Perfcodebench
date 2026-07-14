public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    int len = keys.length;
    long prime = 1099511628211L;
    long offset = 1469598103934665603L;

    for (int iter = 0; iter < iters; iter++) {
      long[] hist = new long[buckets];

      int i = 0;
      int limit = len - 3;
      for (; i < limit; i += 4) {
        hist[keys[i]] += vals[i];
        hist[keys[i+1]] += vals[i+1];
        hist[keys[i+2]] += vals[i+2];
        hist[keys[i+3]] += vals[i+3];
      }
      for (; i < len; i++) {
        hist[keys[i]] += vals[i];
      }

      h = offset;
      int b = 0;
      int bLimit = buckets - 3;
      for (; b < bLimit; b += 4) {
        h ^= (hist[b] + 1); h *= prime;
        h ^= (hist[b+1] + 1); h *= prime;
        h ^= (hist[b+2] + 1); h *= prime;
        h ^= (hist[b+3] + 1); h *= prime;
      }
      for (; b < buckets; b++) {
        h ^= (hist[b] + 1); h *= prime;
      }
    }
    return h;
  }
}