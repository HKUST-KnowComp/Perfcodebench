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
                // To handle the tie-break (smaller index first), we use (n - 1 - index) 
                // or simply store index and handle it in the comparison.
                // Better: store score as positive and index as negative to use natural long order.
                // Since scores can be negative, we map them to unsigned space by adding Integer.MAX_VALUE or XORing.
                // Actually, the simplest way to maintain (score DESC, index ASC) is:
                // long val = ((long)scores[i] << 32) | (0xFFFFFFFFL - i);
                // But scores are signed. Let's use: 
                // high 32 bits: score (inverted for DESC), low 32 bits: index (for ASC).
                // Wait, the baseline is (score DESC, index ASC).
                // Let's use: packed[i] = ((long)scores[i] << 32) | (0xFFFFFFFFL - i);
                // Then we want the LARGEST values.
                packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL - i);
            }

            quickSelect(packed, 0, n - 1, k);

            // The top k are now in packed[n-k ... n-1] but not necessarily sorted.
            // The problem requires the checksum over the ORDERED winners.
            // So we must sort the top k.
            Arrays.sort(packed, n - k, n);

            h = 1469598103934665603L;
            for (int i = n - k; i < n; i++) {
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
        int rank = pivotIndex - left + 1;
        if (k < rank) {
            quickSelect(a, left, pivotIndex - 1, k);
        } else if (k > rank) {
            quickSelect(a, pivotIndex + 1, right, k - rank);
        } else {
            // We found the k-th smallest, but we need the top k (largest).
            // This logic is for k-th smallest. For top-k largest, we need n-k smallest.
        }
    }

    // Redoing the logic to specifically target the top-k largest elements
    private static void quickSelectTopK(long[] a, int left, int right, int k) {
        int target = a.length - k;
        while (left < right) {
            int pivotIndex = partition(a, left, right);
            if (pivotIndex == target) return;
            if (pivotIndex < target) left = pivotIndex + 1;
            else right = pivotIndex - 1;
        }
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

    // Corrected run method to use the iterative quickSelectTopK
    public static long runFixed(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                packed[i] = ((long) scores[i] << 32) | (0xFFFFFFFFL - i);
            }
            
            // We want the k largest elements. This means we want to partition 
            // such that elements from index n-k to n-1 are the largest.
            int left = 0, right = n - 1, target = n - k;
            while (left < right) {
                int p = partition(packed, left, right);
                if (p >= target) right = p;
                else left = p + 1;
            }
            
            Arrays.sort(packed, n - k, n);
            
            h = 1469598103934665603L;
            for (int i = n - k; i < n; i++) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = (int) (0xFFFFFFFFL - (val & 0xFFFFFFFFL));
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    // To match the required signature and use the fixed logic:
    public static long run(int[] scores, int k, int iters) {
        return runFixed(scores, k, iters);
    }
}