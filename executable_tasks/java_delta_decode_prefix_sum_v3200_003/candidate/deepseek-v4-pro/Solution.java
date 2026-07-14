public class Solution {
    public static long run(int[] deltas, int iters) {
        int n = deltas.length;
        long base = n * 0x9E3779B1L;
        long total = 0;
        for (int iter = 0; iter < iters; iter++) {
            long prefix = 0;
            long sum_prefix = 0;
            for (int d : deltas) {
                prefix += d;
                sum_prefix += prefix;
            }
            total = sum_prefix + base;
        }
        return total;
    }
}
