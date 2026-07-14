public class Solution {
    public static long run(int[] deltas, int iters) {
        long current = 0;
        long sum = 0;
        int len = deltas.length;
        for (int i = 0; i < len; i++) {
            current += deltas[i];
            sum += current + 0x9E3779B1L;
        }
        return sum * iters;
    }
}