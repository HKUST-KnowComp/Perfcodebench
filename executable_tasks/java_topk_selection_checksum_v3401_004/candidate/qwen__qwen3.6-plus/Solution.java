import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        for (int i = 0; i < n; i++) {
            packed[i] = ((long)(~scores[i]) << 32) | (i & 0xFFFFFFFFL);
        }
        Arrays.sort(packed);
        
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                h ^= ((~(val >> 32)) << 32) | (val & 0xFFFFFFFFL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}