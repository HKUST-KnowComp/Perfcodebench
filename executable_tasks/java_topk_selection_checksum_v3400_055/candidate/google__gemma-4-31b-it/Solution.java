import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for fast sorting/selection
                // High 32 bits: score (inverted to sort descending)
                // Low 32 bits: index (natural order)
                // We use ~score to make larger scores smaller, then sort ascending
                packed[i] = (((long) ~scores[i]) << 32) | (i & 0xFFFFFFFFL);
            }
            
            // Partial sort to find top k
            quickSelect(packed, 0, n - 1, k);
            
            // The first k elements are now the top k, but not necessarily sorted among themselves
            // The problem requires the checksum over the ORDERED winners.
            // So we must sort the resulting k elements.
            Arrays.sort(packed, 0, k);
            
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                long val = packed[i];
                int score = ~(int)(val >> 32);
                int index = (int)(val & 0xFFFFFFFFL);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(long[] a, int left, int right, int k) {
        if (left >= right) return;
        int pivotIndex = partition(a, left, right);
        if (pivotIndex == k) return;
        if (pivotIndex > k) {
            quickSelect(a, left, pivotIndex - 1, k);
        } else {
            quickSelect(a, pivotIndex + 1, right, k);
        }
    }

    private static int partition(long[] a, int left, int right) {
        long pivot = a[right];
        int i = left;
        for (int j = left; j < right; j++) {
            if (a[j] < pivot) {
                long temp = a[i];
                a[i] = a[j];
                a[j] = temp;
                i++;
            }
        }
        long temp = a[i];
        a[i] = a[right];
        a[right] = temp;
        return i;
    }
}