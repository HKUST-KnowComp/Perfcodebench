public class Solution {
    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        final long OFFSET = 1469598103934665603L;
        final long PRIME = 1099511628211L;
        long[] hist = new long[buckets];
        int len = keys.length;
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            // accumulate bucket sums
            for (int i = 0; i < len; i++) {
                hist[keys[i]] += vals[i];
            }
            // compute checksum and clear histogram in one pass
            h = OFFSET;
            for (int i = 0; i < buckets; i++) {
                long v = hist[i];
                hist[i] = 0; // clear for next iteration
                h ^= (v + 1);
                h *= PRIME;
            }
        }
        return h;
    }
}