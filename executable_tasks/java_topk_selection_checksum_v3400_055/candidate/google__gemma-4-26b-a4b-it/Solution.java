import java.util.*;

public class Solution {
    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        int[] sArr = new int[n];
        int[] iArr = new int[n];
        long h = 0;

        for (int iter = 0; iter < iters; iter++) {
            for (int i = 0; i < n; i++) {
                sArr[i] = scores[i];
                iArr[i] = i;
            }

            // Quickselect to partition top-k elements to the front
            quickSelect(sArr, iArr, 0, n - 1, k);

            // The quickselect ensures elements from 0 to k-1 are the top-k,
            // but they are not necessarily sorted among themselves.
            // The requirement specifies 'ordered winners' based on the original sort logic:
            // 1. Score descending
            // 2. Index ascending
            // We must sort the top-k partition to match the checksum.
            sortTopK(sArr, iArr, k);

            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= (((long) sArr[i]) << 32) ^ (iArr[i] & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static void quickSelect(int[] s, int[] i, int left, int right, int k) {
        if (left >= right) return;
        int pivotIdx = partition(s, i, left, right);
        if (pivotIdx == k) return;
        if (pivotIdx > k) {
            quickSelect(s, i, left, pivotIdx - 1, k);
        } else {
            quickSelect(s, i, pivotIdx + 1, right, k);
        }
    }

    private static int partition(int[] s, int[] i, int left, int right) {
        int mid = left + (right - left) / 2;
        // Median-of-three to avoid worst-case
        int pIdx = selectPivot(s, i, left, mid, right);
        swap(s, i, pIdx, right);
        
        int pivotScore = s[right];
        int pivotIdxVal = i[right];
        int storeIdx = left;

        for (int j = left; j < right; j++) {
            // Sort criteria: Score DESC, Index ASC
            if (s[j] > pivotScore || (s[j] == pivotScore && i[j] < pivotIdxVal)) {
                swap(s, i, j, storeIdx);
                storeIdx++;
            }
        }
        swap(s, i, storeIdx, right);
        return storeIdx;
    }

    private static int selectPivot(int[] s, int[] i, int a, int b, int c) {
        if (compare(s[a], i[a], s[b], i[b]) < 0 && compare(s[a], i[a], s[c], i[c]) < 0) return a;
        if (compare(s[b], i[b], s[a], i[a]) < 0 && compare(s[b], i[b], s[c], i[c]) < 0) return b;
        return c;
    }

    private static int compare(int s1, int i1, int s2, int i2) {
        if (s1 != s2) return Integer.compare(s1, s2); // Higher score is 'smaller' in rank logic
        return Integer.compare(i2, i1); // Lower index is 'smaller' in rank logic
    }

    // Re-implementing the specific comparison for the final sort
    private static void sortTopK(int[] s, int[] i, int k) {
        // We need to sort the first k elements using the exact logic:
        // Score DESC, then Index ASC.
        // Using a simple Dual-Pivot Quicksort or similar via a wrapper is slow due to boxing.
        // Since k is usually small or we need speed, we use a manual quicksort.
        if (k <= 1) return;
        quickSort(s, i, 0, k - 1);
    }

    private static void quickSort(int[] s, int[] i, int left, int right) {
        if (left >= right) return;
        int pivotIdx = partitionForSort(s, i, left, right);
        quickSort(s, i, left, pivotIdx - 1);
        quickSort(s, i, pivotIdx + 1, right);
    }

    private static int partitionForSort(int[] s, int[] i, int left, int right) {
        int mid = left + (right - left) / 2;
        int pIdx = selectPivotForSort(s, i, left, mid, right);
        swap(s, i, pIdx, right);
        int pS = s[right];
        int pI = i[right];
        int store = left;
        for (int j = left; j < right; j++) {
            // Logic: Score DESC, Index ASC
            if (s[j] > pS || (s[j] == pS && i[j] < pI)) {
                swap(s, i, j, store);
                store++;
            }
        }
        swap(s, i, store, right);
        return store;
    }

    private static int selectPivotForSort(int[] s, int[] i, int a, int b, int c) {
        if (isBetter(s[a], i[a], s[b], i[b]) && isBetter(s[a], i[a], s[c], i[c])) return a;
        if (isBetter(s[b], i[b], s[a], i[a]) && isBetter(s[b], i[b], s[c], i[c])) return b;
        return c;
    }

    private static boolean isBetter(int s1, int i1, int s2, int i2) {
        if (s1 != s2) return s1 > s2;
        return i1 < i2;
    }

    private static void swap(int[] s, int[] i, int a, int b) {
        int tempS = s[a];
        s[a] = s[b];
        s[b] = tempS;
        int tempI = i[a];
        i[a] = i[b];
        i[b] = tempI;
    }
}