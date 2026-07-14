public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    // Match baseline behavior: if no iterations, return 0
    if (iters <= 0) return 0L;

    long[] hist = new long[buckets];

    int n = keys.length;
    // Unrolled aggregation loop
    int i = 0;
    int unrollLimit = n - 4;
    for (; i <= unrollLimit; i += 4) {
      int k0 = keys[i];
      int k1 = keys[i + 1];
      int k2 = keys[i + 2];
      int k3 = keys[i + 3];
      hist[k0] += vals[i];
      hist[k1] += vals[i + 1];
      hist[k2] += vals[i + 2];
      hist[k3] += vals[i + 3];
    }
    for (; i < n; i++) {
      hist[keys[i]] += vals[i];
    }

    long h = 1469598103934665603L;
    final long prime = 1099511628211L;

    // Unrolled hashing loop
    int j = 0;
    int hashUnrollLimit = buckets - 4;
    for (; j <= hashUnrollLimit; j += 4) {
      long v0 = hist[j] + 1L;
      h ^= v0;
      h *= prime;
      long v1 = hist[j + 1] + 1L;
      h ^= v1;
      h *= prime;
      long v2 = hist[j + 2] + 1L;
      h ^= v2;
      h *= prime;
      long v3 = hist[j + 3] + 1L;
      h ^= v3;
      h *= prime;
    }
    for (; j < buckets; j++) {
      long v = hist[j] + 1L;
      h ^= v;
      h *= prime;
    }

    return h;
  }
}
