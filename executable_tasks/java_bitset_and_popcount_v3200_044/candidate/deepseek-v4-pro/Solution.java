public class Solution {
    public static long run(long[] left, long[] right, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        long total = 0L;
        for (int i = 0; i < left.length; i++) {
            total += Long.bitCount(left[i] & right[i]);
        }
        return total;
    }
}
