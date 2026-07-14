import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for efficient sorting.
                // Score is shifted to the top. To handle the tie-break (smaller index first),
                // we use (n - 1 - i) or similar, but a simpler way is to store 
                // score in high bits and (MAX_INT - index) in low bits.
                // However, since we need descending score and ascending index:
                // We can use: (long)score << 32 | (0xFFFFFFFFL - i)
                // Then sorting descending will give us high score and low index.
                packed[i] = (((long) scores[i]) << 32) | (0xFFFFFFFFL - i);
            }

            // Partial sort to get top k elements
            quickSelect(packed, 0, n - 1, k);
            
            // Sort only the top k elements to ensure correct order for checksum
            Arrays.sort(packed, 0, k);
            // Since Arrays.sort is ascending, the top k are now at the end of the [0, k) range
            // if we used the packing above. Let's refine the packing for easier sorting.
        }
        return 0; // Placeholder for the logic below
    }

    // Re-implementing with a more robust approach to ensure correctness and performance
    public static long runFixed(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // We want: Higher score first, then Lower index first.
                // Map score to a value where higher is better, and index to a value where lower is better.
                // Use: (score << 32) | (~i & 0xFFFFFFFFL)
                // Then the largest values in the packed array are the winners.
                packed[i] = (((long) scores[i]) << 32) | (~i & 0xFFFFFFFFL);
            }

            // Use a selection algorithm to move the top k to the end of the array
            nthElement(packed, n - k);
            
            // Sort the top k elements (which are now from n-k to n-1) in ascending order
            // to get them in the order of the smallest winner to largest winner,
            // then iterate backwards to get the top-k in descending order.
            Arrays.sort(packed, n - k, n);

            h = 1469598103934665603L;
            for (int i = n - 1; i >= n - k; i--) {
                long val = packed[i];
                int score = (int) (val >>> 32);
                int index = ~((int) val);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void nthElement(long[] arr, int k) {
        int left = 0, right = arr.length - 1;
        Random rnd = new Random();
        while (left < right) {
            int pivotIndex = left + rnd.nextInt(right - left + 1);
            pivotIndex = partition(arr, left, right, pivotIndex);
            if (pivotIndex == k) return;
            if (pivotIndex < k) left = pivotIndex + 1;
            else right = pivotIndex - 1;
        }
    }

    private static int partition(long[] arr, int left, int right, int pivotIndex) {
        long pivotValue = arr[pivotIndex];
        swap(arr, pivotIndex, right);
        int storeIndex = left;
        for (int i = left; i < right; i++) {
            if (arr[i] < pivotValue) {
                swap(arr, i, storeIndex);
                storeIndex++;
            }
        }
        swap(arr, storeIndex, right);
        return storeIndex;
    }

    private static void swap(long[] arr, int i, int j) {
        long temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    // The actual entry point required by the harness
    public static long run(int[] scores, int k, int iters) {
        return runFixed(scores, k, iters);
    }
}