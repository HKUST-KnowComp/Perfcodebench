public class Solution {
    public static long run(int[] keys, int[] vals, int buckets, int iters) {
        if (iters <= 0) return 0L;
        long[] hist = new long[buckets];
        int[] k = keys;
        int[] v = vals;
        int n = k.length;
        for (int i = 0; i < n; i++) {
            hist[k[i]] += v[i];
        }
        long h = 1469598103934665603L;
        int b = buckets;
        long[] hh = hist;
        for (int i = 0; i < b; i++) {
            h ^= (hh[i] + 1);
            h *= 1099511628211L;
        }
        return h;
    }
}
