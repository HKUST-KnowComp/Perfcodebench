public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        long h = 0;
        final long[] hist = new long[buckets];
        final int n = keys.length;
        for (int iter = 0; iter < iters; iter++) {
            // Aggregate values into reusable histogram
            for (int i = 0; i < n; i++) {
                hist[keys[i]] += vals[i];
            }
            // Compute identical FNV-1a checksum as baseline
            h = FNV_OFFSET;
            for (long v : hist) {
                h ^= (v + 1);
                h *= FNV_PRIME;
            }
            // Reset histogram to zero by reversing additions, only touches used indices
            for (int i = 0; i < n; i++) {
                hist[keys[i]] -= vals[i];
            }
        }
        return h;
    }
}