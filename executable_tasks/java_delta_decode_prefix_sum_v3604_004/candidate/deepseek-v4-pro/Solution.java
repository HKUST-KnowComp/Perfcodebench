public class Solution {
    public static long run(int[] deltas, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        long sum = 0L;
        long current = 0L;
        final long magic = 0x9E3779B1L;
        for (int i = 0; i < deltas.length; ++i) {
            current += deltas[i];
            sum += current + magic;
        }
        return sum;
    }
}
