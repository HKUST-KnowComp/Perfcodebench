public class Solution {
    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long hash = 0;
        int n = ids.length;
        for (int iter = 0; iter < iters; iter++) {
            long h = 1469598103934665603L;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                int t = ts[i];
                short lvl = levels[i];
                short len = lens[i];
                // id (4 bytes, little-endian)
                h ^= (id & 0xFFL);
                h *= 1099511628211L;
                id >>>= 8;
                h ^= (id & 0xFFL);
                h *= 1099511628211L;
                id >>>= 8;
                h ^= (id & 0xFFL);
                h *= 1099511628211L;
                id >>>= 8;
                h ^= (id & 0xFFL);
                h *= 1099511628211L;
                // ts (4 bytes)
                h ^= (t & 0xFFL);
                h *= 1099511628211L;
                t >>>= 8;
                h ^= (t & 0xFFL);
                h *= 1099511628211L;
                t >>>= 8;
                h ^= (t & 0xFFL);
                h *= 1099511628211L;
                t >>>= 8;
                h ^= (t & 0xFFL);
                h *= 1099511628211L;
                // level (2 bytes)
                h ^= (lvl & 0xFFL);
                h *= 1099511628211L;
                lvl >>>= 8;
                h ^= (lvl & 0xFFL);
                h *= 1099511628211L;
                // len (2 bytes)
                h ^= (len & 0xFFL);
                h *= 1099511628211L;
                len >>>= 8;
                h ^= (len & 0xFFL);
                h *= 1099511628211L;
            }
            hash = h;
        }
        return hash;
    }
}