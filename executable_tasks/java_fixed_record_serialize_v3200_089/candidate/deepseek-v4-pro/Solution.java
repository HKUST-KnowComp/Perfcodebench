public class Solution {
    private static final long HASH_INIT = 1469598103934665603L;
    private static final long HASH_MUL  = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        long h = HASH_INIT;
        int n = ids.length;
        int[] idArr   = ids;
        int[] tsArr   = ts;
        short[] lvArr = levels;
        short[] lnArr = lens;

        for (int i = 0; i < n; ++i) {
            int id = idArr[i];
            h ^= (id        & 0xFF); h *= HASH_MUL;
            h ^= ((id >>> 8)  & 0xFF); h *= HASH_MUL;
            h ^= ((id >>> 16) & 0xFF); h *= HASH_MUL;
            h ^= ((id >>> 24) & 0xFF); h *= HASH_MUL;

            int t = tsArr[i];
            h ^= (t         & 0xFF); h *= HASH_MUL;
            h ^= ((t >>> 8)  & 0xFF); h *= HASH_MUL;
            h ^= ((t >>> 16) & 0xFF); h *= HASH_MUL;
            h ^= ((t >>> 24) & 0xFF); h *= HASH_MUL;

            short lv = lvArr[i];
            h ^= (lv         & 0xFF); h *= HASH_MUL;
            h ^= ((lv >>> 8)  & 0xFF); h *= HASH_MUL;

            short ln = lnArr[i];
            h ^= (ln         & 0xFF); h *= HASH_MUL;
            h ^= ((ln >>> 8)  & 0xFF); h *= HASH_MUL;
        }
        return h;
    }
}
