public class Solution {
    private static final long INITIAL_HASH = 1469598103934665603L;
    private static final long MULTIPLIER = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long hash = 0;
        int n = ids.length;
        for (int iter = 0; iter < iters; iter++) {
            long h = INITIAL_HASH;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                h ^= (id & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((id >>> 8) & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((id >>> 16) & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((id >>> 24) & 0xFFL);
                h *= MULTIPLIER;

                int t = ts[i];
                h ^= (t & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((t >>> 8) & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((t >>> 16) & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((t >>> 24) & 0xFFL);
                h *= MULTIPLIER;

                short level = levels[i];
                h ^= (level & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((level >>> 8) & 0xFFL);
                h *= MULTIPLIER;

                short len = lens[i];
                h ^= (len & 0xFFL);
                h *= MULTIPLIER;
                h ^= ((len >>> 8) & 0xFFL);
                h *= MULTIPLIER;
            }
            hash = h;
        }
        return hash;
    }
}