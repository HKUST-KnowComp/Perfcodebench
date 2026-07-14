public class Solution {
    private static final long FNV_OFFSET = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] ids, int[] dictVals, int iters) {
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            h = FNV_OFFSET;
            for (int id : ids) {
                int v = dictVals[id];
                h ^= (v & 0xffffffffL) + 1L;
                h *= FNV_PRIME;
            }
        }
        return h;
    }
}