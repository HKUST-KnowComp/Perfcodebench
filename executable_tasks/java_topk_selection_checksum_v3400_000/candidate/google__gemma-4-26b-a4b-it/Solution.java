import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        int[] currentScores = new int[n];
        int[] currentIndices = new int[n];
        long h = 0;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                currentScores[i] = scores[i];
                currentIndices[i] = i;
            }

            // Quickselect to partition the top k elements to the front
            quickSelect(currentScores, currentIndices, 0, n - 1, k);

            // The quickselect might not leave the top k in perfect sorted order
            // according to the specific tie-breaking rules (score desc, index asc).
            // We must sort only the top k elements to ensure checksum correctness.
            sortTopK(currentScores, currentIndices, k);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= (((long) currentScores[i]) << 32) ^ (currentIndices[i] & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(int[] scores, int[] indices, int left, int right, int k) {
        while (left < right) {
            int pivotIndex = partition(scores, indices, left, right);
            if (pivotIndex == k) return;
            if (pivotIndex < k) {
                left = pivotIndex + 1;
            } else {
                right = pivotIndex - 1;
            }
        }
    }

    private static int partition(int[] scores, int[] indices, int left, int right) {
        int mid = left + (right - left) / 2;
        // Simple median-of-three or just middle element for pivot
        swap(scores, indices, mid, right);
        int pivotScore = scores[right];
        int pivotIdx = indices[right];

        int storeIndex = left;
        for (int i = left; i < right; i++) {
            // Sort criteria: score DESC, index ASC
            if (compare(scores[i], indices[i], pivotScore, pivotIdx) < 0) {
                swap(scores, indices, i, storeIndex);
                storeIndex++;
            }
        }
        swap(scores, indices, storeIndex, right);
        return storeIndex;
    }

    private static int compare(int s1, int i1, int s2, int i2) {
        if (s1 != s2) return Integer.compare(s2, s1); // Descending score
        return Integer.compare(i1, i2); // Ascending index
    }

    private static void sortTopK(int[] scores, int[] indices, int k) {
        // Use a custom sort for the first k elements to handle the specific tie-breaking
        // Since k is usually small, a simple insertion sort or Dual-Pivot Quicksort works.
        // For simplicity and correctness, we use a boxed sort or manual quicksort.
        // Here we implement a simple quicksort for the top k.
        if (k <= 1) return;
        quickSort(scores, indices, 0, k - 1);
    }

    private static void quickSort(int[] scores, int[] indices, int left, int right) {
        if (left >= right) return;
        int pivotIdx = partitionForSort(scores, indices, left, right);
        quickSort(scores, indices, left, pivotIdx - 1);
        quickSort(scores, indices, pivotIdx + 1, right);
    }

    private static int partitionForSort(int[] scores, int[] indices, int left, int right) {
        int mid = left + (right - left) / 2;
        swap(scores, indices, mid, right);
        int pS = scores[right];
        int pI = indices[right];
        int i = left;
        for (int j = left; j < right; j++) {
            if (compare(scores[j], indices[j], pS, pI) < 0) {
                swap(scores, indices, i, j);
                i++;
            }
        }
        swap(scores, indices, i, right);
        return i;
    }

    private static void swap(int[] scores, int[] indices, int i, int j) {
        int tempS = scores[i];
        scores[i] = scores[j];
        scores[j] = tempS;
        int tempI = indices[i];
        indices[i] = indices[j];
        indices[j] = tempI;
    }
}