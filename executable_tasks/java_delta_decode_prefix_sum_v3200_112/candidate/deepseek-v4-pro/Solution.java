public class Solution {
    public static long run(int[] deltas, int iters) {
        if (iters == 0) return 0L;
        int n = deltas.length;
        long sum = 0L;
        for (int i = 0; i < n; i++) {
            sum += (long)(n - i) * deltas[i];
        }
        sum += 0x9E3779B1L * n;
        return sum;
    }
}