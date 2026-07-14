public class Solution {
    private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long resultHash = 0;
        int n = ids.length;
        for (int iter = 0; iter < iters; iter++) {
            long hash = FNV_OFFSET_BASIS;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                hash ^= (id & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((id >>> 8) & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((id >>> 16) & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((id >>> 24) & 0xFFL);
                hash *= FNV_PRIME;

                int t = ts[i];
                hash ^= (t & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((t >>> 8) & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((t >>> 16) & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((t >>> 24) & 0xFFL);
                hash *= FNV_PRIME;

                short lvl = levels[i];
                hash ^= (lvl & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((lvl >>> 8) & 0xFFL);
                hash *= FNV_PRIME;

                short len = lens[i];
                hash ^= (len & 0xFFL);
                hash *= FNV_PRIME;
                hash ^= ((len >>> 8) & 0xFFL);
                hash *= FNV_PRIME;
            }
            resultHash = hash;
        }
        return resultHash;
    }
}