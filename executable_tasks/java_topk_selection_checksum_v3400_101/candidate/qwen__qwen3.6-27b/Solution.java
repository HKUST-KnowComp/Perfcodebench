import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        long[] packed = new long[scores.length];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < scores.length; i++) {
                packed[i] = ((long)~scores[i] << 32) | (i & 0xFFFFFFFFL);
            }
            Arrays.sort(packed);
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                int score = (int)~(val >>> 32);
                int index = (int)val;
                h ^= (((long)score) << 32) ^ (index & 0xFFFFFFFFL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}