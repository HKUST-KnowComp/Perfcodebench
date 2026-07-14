import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        long mask = 0xFFFFFFFF00000000L;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                packed[i] = ((long) ~scores[i] << 32) | i;
            }
            Arrays.sort(packed);
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= packed[i] ^ mask;
                h *= 1099511628211L;
            }
        }
        return h;
    }
}