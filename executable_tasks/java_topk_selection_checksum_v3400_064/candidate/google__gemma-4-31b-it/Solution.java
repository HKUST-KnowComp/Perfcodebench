import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for primitive sorting.
                // Score is shifted to high bits. To handle the index tie-break (ascending index),
                // we store (max_index - index) or simply use a bit-trick.
                // Since we want descending score and ascending index:
                // We use (score << 32) and then handle the index carefully.
                // A better way: map score to unsigned-like space and index to inverted space.
                // Or: use a custom Quickselect that handles the tie-break.
                long s = scores[i];
                // Shift score to be positive for comparison, and invert index
                // We want: (s1 > s2) || (s1 == s2 && i1 < i2)
                // We can store: ((long)s << 32) | ((long)(n - 1 - i))
                // Then the largest packed value is the top element.
                packed[i] = ((long) s << 32) | (n - 1 - i);
            }

            quickSelect(packed, 0, n - 1, k);

            // The top k are now in packed[n-k ... n-1] but not necessarily sorted.
            // The problem requires the checksum over the ORDERED winners.
            // So we must sort the top k elements.
            Arrays.sort(packed, n - k, n);

            h = 1469598103934665603L;
            for (int i = n - k; i < n; i++) {
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
        if (left >= right) return;
        int pivotIndex = partition(a, left, right);
        int rank = pivotIndex - left + 1;
        if (k < rank) {
            quickSelect(a, left, pivotIndex - 1, k);
        } else if (k > rank) {
            quickSelect(a, pivotIndex + 1, right, k - rank);
        } else {
            // We found the k-th smallest, but we need the top k (largest).
            // Actually, the logic above is for k-th smallest. 
            // To get top k largest, we need the (n-k)-th smallest.
        }
    }

    // Redefining to specifically find the (n-k)-th smallest element
    private static void quickSelectTopK(long[] a, int left, int right, int targetIdx) {
        while (left < right) {
            int pivotIndex = partition(a, left, right);
            if (pivotIndex == targetIdx) return;
            if (pivotIndex < targetIdx) left = pivotIndex + 1;
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

    // Overriding run to use the corrected QuickSelect logic
    public static long runFixed(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                packed[i] = ((long) scores[i] << 32) | (n - 1 - i);
            }
            // We want the k largest elements. This means we want to partition 
            // such that elements from index (n-k) to (n-1) are the largest.
            int target = n - k;
            int left = 0, right = n - 1;
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
                int index = n - 1 - (int) (val & 0xFFFFFFFFL);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    // To match the required entrypoint exactly, we use the logic from runFixed inside run
    public static long run(int[] scores, int k, int iters) {
        return runFixed(scores, k, iters);
    }
}