import java.util.*;

public class Solution {
    // Unused but kept for compatibility; not called in optimized path
    static long checksumBytes(byte[] data) {
        long hash = 1469598103934665603L;
        for (byte b : data) {
            hash ^= (b & 0xFFL);
            hash *= 1099511628211L;
        }
        return hash;
    }

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        final long seed = 1469598103934665603L;
        final long mult = 1099511628211L;
        long finalHash = 0;
        int n = ids.length;

        for (int iter = 0; iter < iters; iter++) {
            long hash = seed;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                hash ^= (id & 0xFFL);
                hash *= mult;
                hash ^= ((id >>> 8) & 0xFFL);
                hash *= mult;
                hash ^= ((id >>> 16) & 0xFFL);
                hash *= mult;
                hash ^= ((id >>> 24) & 0xFFL);
                hash *= mult;

                int t = ts[i];
                hash ^= (t & 0xFFL);
                hash *= mult;
                hash ^= ((t >>> 8) & 0xFFL);
                hash *= mult;
                hash ^= ((t >>> 16) & 0xFFL);
                hash *= mult;
                hash ^= ((t >>> 24) & 0xFFL);
                hash *= mult;

                short lv = levels[i];
                hash ^= (lv & 0xFFL);
                hash *= mult;
                hash ^= ((lv >>> 8) & 0xFFL);
                hash *= mult;

                short ln = lens[i];
                hash ^= (ln & 0xFFL);
                hash *= mult;
                hash ^= ((ln >>> 8) & 0xFFL);
                hash *= mult;
            }
            finalHash = hash;
        }
        return finalHash;
    }
}