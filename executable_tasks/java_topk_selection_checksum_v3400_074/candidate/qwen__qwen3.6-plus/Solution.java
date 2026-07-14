import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        long[] packed = new long[scores.length];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < scores.length; i++) {
                packed[i] = ((long) ~scores[i] << 32) | i;
            }
            Arrays.sort(packed);
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long p = packed[i];
                int score = ~(int)(p >> 32);
                int index = (int) p;
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}