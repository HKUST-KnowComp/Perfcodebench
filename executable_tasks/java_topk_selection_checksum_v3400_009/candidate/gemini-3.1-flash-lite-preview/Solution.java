import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        for (int i = 0; i < n; i++) {
            // Pack score in high bits, index in low bits. 
            // Negate score to use standard ascending sort for descending order.
            packed[i] = (((long) -scores[i]) << 32) | (i & 0xffffffffL);
        }

        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            // Quickselect to partition top k
            if (k > 0 && k < n) {
                quickSelect(packed, 0, n - 1, k - 1);
            }
            
            // Sort only the top k
            Arrays.sort(packed, 0, k);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                int score = (int) (-(val >> 32));
                int index = (int) (val & 0xffffffffL);
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
            if (arr[j] < pivot) {
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