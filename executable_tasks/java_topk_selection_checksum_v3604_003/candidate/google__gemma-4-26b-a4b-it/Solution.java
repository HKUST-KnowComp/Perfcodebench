import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        int[] indices = new int[n];
        for (int i = 0; i < n; i++) indices[i] = i;

        long h = 1469598103934665603L;
        Random rnd = new Random(42);

        for (int iter = 0; iter < iters; iter++) {
            quickSelect(scores, indices, 0, n - 1, k, rnd);
            
            // The top k elements are now in indices[0...k-1], but they might not be sorted.
            // The requirement is the checksum over the ORDERED winners.
            // We must sort only the top k elements.
            sortTopK(scores, indices, k);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                int idx = indices[i];
                h ^= (((long) scores[idx]) << 32) ^ (idx & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(int[] scores, int[] indices, int left, int right, int k, Random rnd) {
        while (right > left) {
            int pivotIndex = left + rnd.nextInt(right - left + 1);
            pivotIndex = partition(scores, indices, left, right, pivotIndex);
            if (pivotIndex == k) return;
            if (pivotIndex < k) left = pivotIndex + 1;
            else right = pivotIndex - 1;
        }
    }

    private static int partition(int[] scores, int[] indices, int left, int right, int pivotIndex) {
        int pivotScore = scores[indices[pivotIndex]];
        int pivotIdxVal = indices[pivotIndex];
        
        swap(indices, pivotIndex, right);
        int storeIndex = left;
        
        for (int i = left; i < right; i++) {
            if (compare(scores[indices[i]], indices[i], pivotScore, pivotIdxVal) < 0) {
                swap(indices, i, storeIndex);
                storeIndex++;
            }
        }
        swap(indices, storeIndex, right);
        return storeIndex;
    }

    private static void sortTopK(int[] scores, int[] indices, int k) {
        // Use a simple insertion sort or dual-pivot for the small k
        // Since k is usually small, insertion sort is very fast.
        for (int i = 1; i < k; i++) {
            int currentIdx = indices[i];
            int currentScore = scores[currentIdx];
            int j = i - 1;
            while (j >= 0 && compare(currentScore, currentIdx, scores[indices[j]], indices[j]) < 0) {
                indices[j + 1] = indices[j];
                j--;
            }
            indices[j + 1] = currentIdx;
        }
    }

    private static int compare(int s1, int i1, int s2, int i2) {
        if (s1 != s2) return Integer.compare(s2, s1); // Descending score
        return Integer.compare(i1, i2); // Ascending index
    }

    private static void swap(int[] arr, int i, int j) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }
}