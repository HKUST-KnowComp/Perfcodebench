public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    final int n = keys.length;
    // Handle zero buckets: hash loop does nothing, but baseline resets h to offset each iter
    if (buckets <= 0) {
      long h = 0L;
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }

    // Reuse arrays across iterations and avoid clearing via epoch tagging
    final long[] hist = new long[buckets];
    final int[] tags = new int[buckets]; // default 0
    int epoch = 1; // start from 1 so default 0 means "untouched this epoch"

    final int[] lkeys = keys;
    final int[] lvals = vals;
    final long offset = 1469598103934665603L;
    final long prime = 1099511628211L;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      // Build histogram for this iteration with lazy reset on first touch
      for (int i = 0; i < n; i++) {
        int k = lkeys[i];
        if (tags[k] != epoch) {
          tags[k] = epoch;
          hist[k] = 0L;
        }
        hist[k] += lvals[i];
      }

      // Compute checksum over all buckets, treating untagged as zero
      long hh = offset;
      for (int b = 0; b < buckets; b++) {
        long v = (tags[b] == epoch) ? hist[b] : 0L;
        hh ^= (v + 1L);
        hh *= prime;
      }
      h = hh; // last iteration's hash is returned

      // Advance epoch; handle rare wraparound by resetting tags
      epoch++;
      if (epoch == 0) {
        java.util.Arrays.fill(tags, 0);
        epoch = 1;
      }
    }

    return h;
  }
}
