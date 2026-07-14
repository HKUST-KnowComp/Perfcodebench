public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        long h = 0;
        long[] hist = new long[buckets];
        final int len = keys.length;
        for (int iter = 0; iter < iters; iter++) {
            // Build histogram
            for (int i = 0; i < len; i++) {
                hist[keys[i]] += vals[i];
            }
            // Compute required FNV-1a checksum
            h = FNV_OFFSET;
            for (long v : hist) {
                h ^= (v + 1);
                h *= FNV_PRIME;
            }
            // Reset histogram to zero for next iteration
            for (int i = 0; i < len; i++) {
                hist[keys[i]] -= vals[i];
            }
        }
        return h;
    }
}