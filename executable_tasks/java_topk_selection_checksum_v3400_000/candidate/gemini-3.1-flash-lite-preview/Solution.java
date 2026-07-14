import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        int[] indices = new int[n];
        for (int i = 0; i < n; i++) indices[i] = i;

        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            // Quickselect to partition top k
            select(scores, indices, 0, n - 1, k);
            
            // Sort only the top k
            sortTopK(scores, indices, 0, k - 1);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                int idx = indices[i];
                h ^= (((long) scores[idx]) << 32) ^ (idx & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void select(int[] scores, int[] indices, int left, int right, int k) {
        while (left < right) {
            int pivotIndex = partition(scores, indices, left, right);
            if (pivotIndex == k) return;
            if (pivotIndex < k) left = pivotIndex + 1;
            else right = pivotIndex - 1;
        }
    }

    private static int partition(int[] scores, int[] indices, int left, int right) {
        int mid = (left + right) >>> 1;
        swap(indices, mid, right);
        int pivotIdx = indices[right];
        int i = left;
        for (int j = left; j < right; j++) {
            if (compare(scores, indices[j], pivotIdx) < 0) {
                swap(indices, i++, j);
            }
        }
        swap(indices, i, right);
        return i;
    }

    private static int compare(int[] scores, int idxA, int idxB) {
        if (scores[idxA] != scores[idxB]) return Integer.compare(scores[idxB], scores[idxA]);
        return Integer.compare(idxA, idxB);
    }

    private static void swap(int[] arr, int i, int j) {
        int temp = arr[i];
        arr[i] = arr[j];
        arr[j] = temp;
    }

    private static void sortTopK(int[] scores, int[] indices, int left, int right) {
        if (left >= right) return;
        int pivot = partition(scores, indices, left, right);
        sortTopK(scores, indices, left, pivot - 1);
        sortTopK(scores, indices, pivot + 1, right);
    }
}