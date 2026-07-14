public class Solution {
    public static long run(int[] deltas, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        int n = deltas.length;
        long prefixSum = 0L;
        long current = 0L;
        for (int i = 0; i < n; i++) {
            current += deltas[i];
            prefixSum += current;
        }
        return prefixSum + n * 0x9E3779B1L;
    }
}