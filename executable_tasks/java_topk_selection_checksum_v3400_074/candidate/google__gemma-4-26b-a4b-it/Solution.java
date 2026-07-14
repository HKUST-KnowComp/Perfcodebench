import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        int[] s = new int[n];
        int[] idx = new int[n];
        long h = 1469598103934665603L;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                s[i] = scores[i];
                idx[i] = i;
            }

            // Quickselect to partition top k elements
            quickSelect(s, idx, 0, n - 1, k);

            // The top k elements are in s[0...k-1], but they are not necessarily sorted.
            // The requirement specifies the checksum is over the ORDERED winners.
            // We must sort only the top k elements.
            sortTopK(s, idx, k);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= (((long) s[i]) << 32) ^ (idx[i] & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(int[] s, int[] idx, int left, int right, int k) {
        while (left < right) {
            int pivotIdx = partition(s, idx, left, right);
            if (pivotIdx == k) return;
            if (pivotIdx < k) {
                left = pivotIdx + 1;
            } else {
                right = pivotIdx - 1;
            }
        }
    }

    private static int partition(int[] s, int[] idx, int left, int right) {
        int mid = left + (right - left) / 2;
        // Median of three to avoid worst case
        if (compare(s[left], idx[left], s[mid], idx[mid]) > 0) swap(s, idx, left, mid);
        if (compare(s[left], idx[left], s[right], idx[right]) > 0) swap(s, idx, left, right);
        if (compare(s[mid], idx[mid], s[right], idx[right]) > 0) swap(s, idx, mid, right);
        
        int pivotS = s[mid];
        int pivotIdx = idx[mid];
        swap(s, idx, mid, right - 1);
        
        int i = left;
        int j = right - 1;
        while (true) {
            while (compare(s[++i], idx[i], pivotS, pivotIdx) < 0);
            while (compare(s[--j], idx[j], pivotS, pivotIdx) > 0);
            if (i >= j) break;
            swap(s, idx, i, j);
        }
        swap(s, idx, i, right - 1);
        return i;
    }

    private static void sortTopK(int[] s, int[] idx, int k) {
        // Use a simple insertion sort or dual-pivot for the small k range
        // Since k is usually small, insertion sort is very fast.
        for (int i = 1; i < k; i++) {
            int curS = s[i];
            int curIdx = idx[i];
            int j = i - 1;
            while (j >= 0 && compare(s[j], idx[j], curS, curIdx) > 0) {
                s[j + 1] = s[j];
                idx[j + 1] = idx[j];
                j--;
            }
            s[j + 1] = curS;
            idx[j + 1] = curIdx;
        }
    }

    private static int compare(int s1, int i1, int s2, int i2) {
        if (s1 != s2) return Integer.compare(s2, s1); // Descending score
        return Integer.compare(i1, i2); // Ascending index
    }

    private static void swap(int[] s, int[] idx, int i, int j) {
        int tempS = s[i];
        s[i] = s[j];
        s[j] = tempS;
        int tempIdx = idx[i];
        idx[i] = idx[j];
        idx[j] = tempIdx;
    }
}