public class Solution {
    public static long run(int[] ids, int[] dictVals, int iters) {
        long h = 1469598103934665603L;
        for (int i = 0; i < ids.length; i++) {
            int v = dictVals[ids[i]];
            h ^= (v & 0xffffffffL) + 1L;
            h *= 1099511628211L;
        }
        return h;
    }
}