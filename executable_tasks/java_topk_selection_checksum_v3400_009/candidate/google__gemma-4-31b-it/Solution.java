import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for fast sorting.
                // Score is shifted to high bits. 
                // To handle the tie-break (smaller index first), we use (n - 1 - index).
                // This way, a larger value in the low bits corresponds to a smaller index.
                // We use a bitmask to ensure the score is treated as unsigned/shifted correctly.
                long s = scores[i];
                packed[i] = ((s & 0xFFFFFFFFL) << 32) | (n - 1 - i);
            }

            quickSelect(packed, 0, n - 1, k);

            // The top k are now in the first k positions, but not necessarily sorted.
            // The problem requires the checksum over the ORDERED winners.
            // So we must sort the top k elements.
            Arrays.sort(packed, 0, k);
            // Arrays.sort sorts ascending, so the top k (largest) are at the end of the range [0, k).
            // However, our packed format makes larger scores larger values.
            // So the top k are indices [0...k-1] after quickSelect, but we need them sorted descending.
            
            h = 1469598103934665603L;
            for (int i = k - 1; i >= 0; i--) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = n - 1 - (int) (val & 0xFFFFFFFFL);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(long[] a, int left, int right, int k) {
        while (left < right) {
            int pivotIndex = partition(a, left, right);
            if (pivotIndex == k) return;
            if (pivotIndex < k) {
                left = pivotIndex + 1;
            } else {
                right = pivotIndex - 1;
            }
        }
    }

    private static int partition(long[] a, int left, int right) {
        long pivot = a[(left + right) >>> 1];
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

    // Redefining the logic to match the exact sorting requirement: 
    // The baseline sorts by score DESC, then index ASC.
    // My packed long: (score << 32) | (n - 1 - index).
    // A larger packed long means: larger score OR (same score and smaller index).
    // This perfectly maps to the baseline's sorting criteria.
    // To get the top k, we need the k largest packed longs.
    
    // Corrected run method to ensure the top k are selected and sorted correctly.
    public static long runFixed(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                packed[i] = (((long) scores[i]) << 32) | (n - 1 - i);
            }
            // We want the k largest elements. 
            // After this, elements from [n-k, n-1] will be the top k.
            nthElement(packed, 0, n - 1, n - k);
            
            Arrays.sort(packed, n - k, n);
            
            h = 1469598103934665603L;
            for (int i = n - 1; i >= n - k; i--) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = n - 1 - (int) (val & 0xFFFFFFFFL);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void nthElement(long[] a, int left, int right, int n) {
        while (left < right) {
            int i = left, j = right;
            long pivot = a[(i + j) >>> 1];
            while (i <= j) {
                while (a[i] < pivot) i++;
                while (a[j] > pivot) j--;
                if (i <= j) {
                    long temp = a[i]; a[i] = a[j]; a[j] = temp;
                    i++; j--;
                }
            }
            if (n <= j) right = j;
            else if (n >= i) left = i;
            else break;
        }
    }

    // Overriding the entry point to use the fixed logic
    public static long run(int[] scores, int k, int iters) {
        return runFixed(scores, k, iters);
    }
}