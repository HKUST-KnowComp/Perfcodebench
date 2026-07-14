import java.util.Arrays;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        long h = 0;
        long[] hist = new long[buckets];
        for (int iter = 0; iter < iters; iter++) {
            Arrays.fill(hist, 0L);
            for (int i = 0; i < keys.length; i++) {
                hist[keys[i]] += vals[i];
            }
            h = FNV_OFFSET;
            for (long v : hist) {
                h ^= (v + 1);
                h *= FNV_PRIME;
            }
        }
        return h;
    }
}