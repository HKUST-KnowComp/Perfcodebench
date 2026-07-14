import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        long h = 0;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index into a long for fast primitive sorting.
                // Score is shifted to high bits. 
                // To handle the tie-break (lower index first) in a descending sort,
                // we store (MAX_INT - index) in the low bits.
                packed[i] = (((long) scores[i]) << 32) | (0xFFFFFFFFL - i);
            }

            // Use Quickselect to move top k elements to the front
            quickSelect(packed, 0, n - 1, k);

            // The first k elements are the top k, but not necessarily sorted.
            // We must sort them to ensure the checksum is consistent.
            Arrays.sort(packed, 0, k);
            // Arrays.sort sorts ascending, so the top k are now at the end of the range [0, k).
            // We iterate backwards from k-1 to 0 to get descending order.

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
        while (left < right) {
            int pivotIndex = partition(a, left, right);
            if (pivotIndex == k) return;
            if (pivotIndex < k) left = pivotIndex + 1;
            else right = pivotIndex - 1;
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
}