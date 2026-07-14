public class Solution {
    public static long run(int[] deltas, int iters) {
        if (iters <= 0) return 0L;
        final long C = 0x9E3779B1L;
        long sum = 0L;
        long prefix = 0L;
        for (int delta : deltas) {
            prefix += delta;
            sum += prefix;
        }
        return sum + (long) deltas.length * C;
    }
}