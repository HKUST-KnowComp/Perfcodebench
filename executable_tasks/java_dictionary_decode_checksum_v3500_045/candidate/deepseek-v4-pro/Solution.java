public class Solution {
    public static long run(int[] ids, int[] dictVals, int iters) {
        if (iters <= 0) return 0L;
        long h = 1469598103934665603L;
        for (int i = 0; i < ids.length; i++) {
            long v = dictVals[ids[i]] & 0xffffffffL;
            h ^= (v + 1L);
            h *= 1099511628211L;
        }
        return h;
    }
}