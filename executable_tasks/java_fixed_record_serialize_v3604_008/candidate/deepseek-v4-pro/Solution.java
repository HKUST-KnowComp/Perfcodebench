public class Solution {
    private static final long INIT_HASH = 1469598103934665603L;
    private static final long MULT = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long hash = 0;
        for (int iter = 0; iter < iters; iter++) {
            hash = INIT_HASH;
            for (int i = 0; i < ids.length; i++) {
                int id = ids[i];
                hash ^= (id & 0xFFL);
                hash *= MULT;
                hash ^= ((id >>> 8) & 0xFFL);
                hash *= MULT;
                hash ^= ((id >>> 16) & 0xFFL);
                hash *= MULT;
                hash ^= ((id >>> 24) & 0xFFL);
                hash *= MULT;

                int t = ts[i];
                hash ^= (t & 0xFFL);
                hash *= MULT;
                hash ^= ((t >>> 8) & 0xFFL);
                hash *= MULT;
                hash ^= ((t >>> 16) & 0xFFL);
                hash *= MULT;
                hash ^= ((t >>> 24) & 0xFFL);
                hash *= MULT;

                short lv = levels[i];
                hash ^= (lv & 0xFFL);
                hash *= MULT;
                hash ^= ((lv >>> 8) & 0xFFL);
                hash *= MULT;

                short ln = lens[i];
                hash ^= (ln & 0xFFL);
                hash *= MULT;
                hash ^= ((ln >>> 8) & 0xFFL);
                hash *= MULT;
            }
        }
        return hash;
    }
}