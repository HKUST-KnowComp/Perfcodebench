import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for efficient sorting.
                // Score is shifted to high bits. 
                // Since we want descending score and ascending index,
                // we store score as is (but handle sign) and index as (max_int - index).
                // However, a simpler way for descending score and ascending index:
                // Use (long)score << 32 and (long)(Integer.MAX_VALUE - i).
                // But since we need the exact top-k, we can just use a custom comparator logic in Quickselect.
                // To use primitive longs: high 32 bits = score, low 32 bits = -index.
                // This way, larger score is better, and for same score, larger (-index) is better (which means smaller index).
                packed[i] = (((long) scores[i]) << 32) | (0xFFFFFFFFL - i);
            }
            
            quickSelect(packed, 0, n - 1, k);
            
            // The top k are now in packed[0...k-1], but not necessarily sorted.
            // The problem requires the checksum over the ORDERED winners.
            // So we must sort the top k elements.
            Arrays.sort(packed, 0, k);
            // Arrays.sort sorts ascending. Our packing makes the 'best' elements the largest.
            // So the top k are the largest k elements. 
            // Let's refine: we want the k largest elements, then sort them descending.
            
            h = 1469598103934665603L;
            for (int i = k - 1; i >= 0; i--) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(long[] a, int left, int right, int k) {
        if (left >= right) return;
        int pivotIndex = partition(a, left, right);
        if (pivotIndex == nMinusK(a, k)) return;
        if (pivotIndex < nMinusK(a, k)) {
            quickSelect(a, pivotIndex + 1, right, k);
        } else {
            quickSelect(a, left, pivotIndex - 1, k);
        }
    }

    private static int nMinusK(long[] a, int k) {
        return a.length - k;
    }

    private static int partition(long[] a, int left, int right) {
        long pivot = a[left + (right - left) / 2];
        int i = left - 1;
        int j = right + 1;
        while (true) {
            do { i++; } while (a[i] < pivot);
            do { j--; } while (a[j] > pivot);
            if (i >= j) return j;
            long temp = a[i];
            a[i] = a[j];
            a[j] = temp;
        }
    }

    // Redoing the logic to be cleaner and strictly correct regarding the ordering
    public static long runFixed(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack: score in high 32, (MAX_INT - index) in low 32
                // This makes the 'best' (highest score, lowest index) the largest long
                packed[i] = (((long) scores[i]) << 32) | (0xFFFFFFFFL - i);
            }
            
            // We need the k largest elements
            nthElement(packed, 0, n - 1, n - k);
            
            // Sort the k largest elements in descending order
            Arrays.sort(packed, n - k, n);
            
            h = 1469598103934665603L;
            for (int i = n - 1; i >= n - k; i--) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void nthElement(long[] a, int left, int right, int n) {
        while (left < right) {
            int pivotIndex = partition(a, left, right);
            if (pivotIndex == n) return;
            if (pivotIndex < n) left = pivotIndex + 1;
            else right = pivotIndex;
        }
    }

    private static int partition(long[] a, int left, int right) {
        long pivot = a[left + (right - left) / 2];
        int i = left, j = right;
        while (i <= j) {
            while (a[i] < pivot) i++;
            while (a[j] > pivot) j--;
            if (i <= j) {
                long temp = a[i];
                a[i] = a[j];
                a[j] = temp;
                i++;
                j--;
            }
        }
        return i;
    }

    // The actual entry point used by the harness
    public static long run(int[] scores, int k, int iters) {
        return runFixed(scores, k, iters);
    }
}