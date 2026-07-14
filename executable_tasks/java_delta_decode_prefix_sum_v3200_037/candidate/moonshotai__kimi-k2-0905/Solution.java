public class Solution {
    public static long run(int[] deltas, int iters) {
        final int len = deltas.length;
        final long magic = 0x9E3779B1L;
        long total = 0;

        for (int iter = 0; iter < iters; iter++) {
            long current = 0;
            long acc = 0;

            // process 4 elements at a time
            int i = 0;
            for (; i + 3 < len; i += 4) {
                current += deltas[i];     acc += current + magic;
                current += deltas[i + 1]; acc += current + magic;
                current += deltas[i + 2]; acc += current + magic;
                current += deltas[i + 3]; acc += current + magic;
            }

            // tail
            for (; i < len; i++) {
                current += deltas[i];
                acc += current + magic;
            }

            total = acc;
        }
        return total;
    }
}