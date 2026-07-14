import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] combined = new long[n];
        for (int i = 0; i < n; i++) {
            combined[i] = (((long) scores[i]) << 32) | (0xFFFFFFFFL - i);
        }

        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            long[] arr = combined.clone();
            quickSelect(arr, 0, n - 1, k);
            
            long[] topK = new long[k];
            System.arraycopy(arr, 0, topK, 0, k);
            Arrays.sort(topK);
            
            h = 1469598103934665603L;
            for (int i = k - 1; i >= 0; i--) {
                long val = topK[i];
                int score = (int) (val >>> 32);
                int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(long[] arr, int left, int right, int k) {
        while (left < right) {
            int pivotIndex = partition(arr, left, right);
            if (pivotIndex == k) return;
            if (pivotIndex < k) left = pivotIndex + 1;
            else right = pivotIndex - 1;
        }
    }

    private static int partition(long[] arr, int left, int right) {
        long pivot = arr[right];
        int i = left;
        for (int j = left; j < right; j++) {
            if (arr[j] > pivot) {
                long temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
                i++;
            }
        }
        long temp = arr[i];
        arr[i] = arr[right];
        arr[right] = temp;
        return i;
    }
}