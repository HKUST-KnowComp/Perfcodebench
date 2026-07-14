public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long[] hist = new long[buckets];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < keys.length; i++) {
        hist[keys[i]] += vals[i];
      }
      long fnv = 1469598103934665603L;
      for (int i = 0; i < buckets; i++) {
        long v = hist[i];
        hist[i] = 0;
        fnv ^= (v + 1);
        fnv *= 1099511628211L;
      }
      h = fnv;
    }
    return h;
  }
}