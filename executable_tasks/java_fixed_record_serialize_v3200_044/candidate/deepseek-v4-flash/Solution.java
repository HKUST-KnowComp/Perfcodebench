import java.util.*;

public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        int n = ids.length;
        long hash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long h = FNV_OFFSET;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                h ^= (id & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((id >>> 8) & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((id >>> 16) & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((id >>> 24) & 0xFFL);
                h *= FNV_PRIME;

                int t = ts[i];
                h ^= (t & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((t >>> 8) & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((t >>> 16) & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((t >>> 24) & 0xFFL);
                h *= FNV_PRIME;

                int level = levels[i] & 0xFFFF;
                h ^= (level & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((level >>> 8) & 0xFFL);
                h *= FNV_PRIME;

                int len = lens[i] & 0xFFFF;
                h ^= (len & 0xFFL);
                h *= FNV_PRIME;
                h ^= ((len >>> 8) & 0xFFL);
                h *= FNV_PRIME;
            }
            hash = h;
        }
        return hash;
    }
}