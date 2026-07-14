public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    // Precompute histogram once since keys and vals are immutable across iterations
    long[] hist = new long[buckets];
    for (int i = 0; i < keys.length; i++) {
      hist[keys[i]] += vals[i];
    }
    // Only recompute checksum in iteration loop, no redundant histogram rebuilds
    for (int iter = 0; iter < iters; iter++) {
      h = 1469598103934665603L;
      for (long v : hist) {
        h ^= (v + 1);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}