import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        long[] packed = new long[n];
        
        // We want to sort by score descending, then index ascending.
        // To use a standard sort or quickselect on longs:
        // High 32 bits: score (inverted to make it descending)
        // Low 32 bits: index (kept as is for ascending)
        // However, a simpler way to handle the "score desc, index asc" is:
        // packed[i] = ((long)scores[i] << 32) | (n - i);
        // But since we need the exact index, let's use:
        // packed[i] = ((long)scores[i] << 32) | (0xFFFFFFFFL - (long)i);
        // Then the largest packed value corresponds to the highest score and lowest index.
        
        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                // Pack score and index such that natural order matches the required priority
                // Score is primary (desc), Index is secondary (asc).
                // We use (long)scores[i] << 32 and then a transformation on the index.
                // To make index ascending while score is descending, we can't just use a single long
                // unless we flip the bits of the score or the index.
                // Let's use: (score << 32) | (~index)
                packed[i] = ((long) scores[i] << 32) | (~i & 0xFFFFFFFFL);
            }
            
            // Partial sort to get top k elements
            quickSelect(packed, 0, n - 1, k);
            
            // The top k are now in packed[n-k ... n-1]
            // We need them sorted exactly to compute the checksum
            Arrays.sort(packed, n - k, n);
            
            h = 1469598103934665603L;
            for (int i = n - k; i < n; i++) {
                long val = packed[i];
                int score = (int) (val >> 32);
                int index = (int) (~val);
                h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(long[] arr, int left, int right, int k) {
        if (left >= right) return;
        int pivotIndex = partition(arr, left, right);
        if (pivotIndex < (arr.length - k)) {
            quickSelect(arr, pivotIndex + 1, right, k);
        } else if (pivotIndex > (arr.length - k)) {
            quickSelect(arr, left, pivotIndex - 1, k);
        }
    }

    private static int partition(long[] arr, int left, int right) {
        long pivot = arr[left + (right - left) / 2];
        int i = left - 1;
        int j = right + 1;
        while (true) {
            do { i++; } while (arr[i] < pivot);
            do { j--; } while (arr[j] > pivot);
            if (i >= j) return j;
            long temp = arr[i];
            arr[i] = arr[j];
            arr[j] = temp;
        }
    }
}