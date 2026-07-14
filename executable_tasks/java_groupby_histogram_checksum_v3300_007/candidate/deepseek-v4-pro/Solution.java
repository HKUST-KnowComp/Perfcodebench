public class Solution {
    private static final long H_INIT = 1469598103934665603L;
    private static final long MULT = 1099511628211L;

    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        long[] hist = new long[buckets];
        int len = keys.length;
        // Unrolled accumulation
        int i = 0;
        for (; i + 3 < len; i += 4) {
            hist[keys[i]] += vals[i];
            hist[keys[i+1]] += vals[i+1];
            hist[keys[i+2]] += vals[i+2];
            hist[keys[i+3]] += vals[i+3];
        }
        for (; i < len; i++) {
            hist[keys[i]] += vals[i];
        }
        long h = H_INIT;
        // Unrolled checksum mixing
        int j = 0;
        for (; j + 3 < buckets; j += 4) {
            long v0 = hist[j];
            long v1 = hist[j+1];
            long v2 = hist[j+2];
            long v3 = hist[j+3];
            h ^= (v0 + 1); h *= MULT;
            h ^= (v1 + 1); h *= MULT;
            h ^= (v2 + 1); h *= MULT;
            h ^= (v3 + 1); h *= MULT;
        }
        for (; j < buckets; j++) {
            h ^= (hist[j] + 1);
            h *= MULT;
        }
        return h;
    }
}