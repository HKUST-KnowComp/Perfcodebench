import java.util.Arrays;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        // Pack score and index into a long to avoid object allocation.
        // We want descending score, then ascending index.
        // To use standard sort/selection, we transform the score to be sortable.
        // Since we want descending score, we can use (Integer.MAX_VALUE - score) or similar.
        // However, a cleaner way for primitive long: 
        // bits = ((long)score << 32) | (index & 0xFFFFFFFFL);
        // But we need descending score and ascending index.
        // Let's use: bits = ((long)score << 32) | (long)(~index & 0xFFFFFFFFL);
        // Then a standard ascending sort on these longs will give us:
        // 1. Higher scores first (because higher score = higher bits)
        // 2. For same score, lower index first (because ~index is higher for lower index)
        // Wait, if we want descending score, we should use a transformation that makes higher scores smaller or just sort and reverse.
        // Let's use: bits = ((long)score << 32) | (long)(0xFFFFFFFFL ^ (index & 0xFFFFFFFFL));
        // This way, higher score -> higher bits. For same score, lower index -> higher bits.
        // To get top-k (highest scores), we can sort and take the end of the array, or transform scores to be negative.
        
        long[] packed = new long[n];
        long h = 1469598103934665603L;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // We want to sort such that the 'best' elements are at the end for easy access,
                // or just use a standard sort and pick the top.
                // Let's pack: [32 bits score][32 bits inverted index]
                // Inverted index: ~i (to make smaller index 'larger' in bit comparison)
                // We want descending score. Let's use: 
                // bits = ((long)score << 32) | (long)(~i & 0xFFFFFFFFL);
                // A standard ascending sort will put highest scores at the end.
                packed[i] = ((long) scores[i] << 32) | ((long) (~i) & 0xFFFFFFFFL);
            }

            // Use Dual-Pivot Quicksort on primitives (very fast)
            Arrays.sort(packed);

            // The top-k elements are at the end of the array.
            // They are ordered by score descending, then index ascending.
            // Because we used ~i, for the same score, the largest ~i (smallest i) is at the end.
            // So the last k elements are: [..., best_k-1, ..., best_1, best_0]
            // We need to iterate from index n-1 down to n-k to get them in the correct order.
            
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[n - 1 - i];
                int score = (int) (val >>> 32);
                int index = (int) (~(val & 0xFFFFFFFFL));
                
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }
}