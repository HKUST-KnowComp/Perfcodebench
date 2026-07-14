public class Solution {
    public static long run(int[] deltas, int iters) {
        long prefix = 0;
        long total = 0;
        for (int d : deltas) {
            prefix += d;
            total += prefix;
        }
        total += (long) deltas.length * 0x9E3779B1L;
        // The result depends only on deltas, not on iters.
        return total;
    }
}