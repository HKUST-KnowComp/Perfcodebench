import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        
        // We need to sort by score DESC, then by index ASC.
        // To achieve this with a single long sort:
        // High 32 bits: score (inverted to make it ascending for the sort)
        // Low 32 bits: index (ascending)
        // Since we want score DESC, we can use (Integer.MAX_VALUE - score).
        
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack: [32-bit inverted score][32-bit index]
                // Using (0xFFFFFFFFL - scores[i]) ensures that higher scores result in smaller values
                long p = (((long) (0xFFFFFFFFL - scores[i])) << 32) | (i & 0xFFFFFFFFL);
                packed[i] = p;
            }
            
            Arrays.sort(packed);
            
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                // Unpack
                int index = (int) (val & 0xFFFFFFFFL);
                int score = scores[index]; // Original score
                
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}