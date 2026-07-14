import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for efficient sorting.
                // We want descending score, then ascending index.
                // To achieve this with a single ascending sort:
                // 1. Transform score to (Integer.MAX_VALUE - score)
                // 2. Pack index in the lower bits.
                // However, a simpler way for descending score is to flip the sign of the score
                // and use the index as is.
                // Let's use: (long)(-scores[i]) << 32 | (i & 0xFFFFFFFFL)
                packed[i] = (((long) -scores[i]) << 32) | (i & 0xFFFFFFFFL);
            }
            
            Arrays.sort(packed);
            
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                int score = -(int)(val >> 32);
                int index = (int)(val & 0xFFFFFFFFL);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}