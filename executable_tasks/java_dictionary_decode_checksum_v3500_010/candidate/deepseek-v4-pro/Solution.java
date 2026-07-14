public class Solution {
    public static long run(int[] ids, int[] dictVals, int iters) {
        long h = 0;
        int len = ids.length;
        int[] idsLocal = ids;
        int[] dictLocal = dictVals;
        for (int iter = 0; iter < iters; iter++) {
            long hash = 1469598103934665603L;
            for (int i = 0; i < len; i++) {
                int v = dictLocal[idsLocal[i]];
                long uv = (v & 0xFFFFFFFFL) + 1L;
                hash ^= uv;
                hash *= 1099511628211L;
            }
            h = hash;
        }
        return h;
    }
}