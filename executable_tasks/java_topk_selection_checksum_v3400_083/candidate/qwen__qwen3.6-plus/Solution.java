import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        long[] data = new long[scores.length];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < scores.length; i++) {
                data[i] = ((long) (~scores[i]) << 32) | i;
            }
            Arrays.sort(data);
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= data[i] ^ 0xFFFFFFFF00000000L;
                h *= 1099511628211L;
            }
        }
        return h;
    }
}