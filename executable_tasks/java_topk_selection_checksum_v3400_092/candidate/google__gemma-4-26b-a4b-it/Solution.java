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

            // The quickselect ensures elements from 0 to k-1 are the top k,
            // but they are not necessarily sorted among themselves. 
            // The requirement specifies 'ordered winners'. 
            // Based on the baseline, the order is: score DESC, then index ASC.
            
            // We must sort only the top k elements to match the baseline's exact checksum.
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
        while (left < right) {
            int pivotIdx = partition(s, i, left, right);
            if (pivotIdx == k) return;
            if (pivotIdx < k) {
                left = pivotIdx + 1;
            } else {
                right = pivotIdx - 1;
            }
        }
    }

    private static int partition(int[] s, int[] i, int left, int right) {
        int mid = left + (right - left) / 2;
        // Use median-of-three or similar to avoid worst case, but simple mid is okay for most
        swap(s, i, mid, right);
        int pivotS = s[right];
        int pivotI = i[right];
        
        int storeIdx = left;
        for (int j = left; j < right; j++) {
            // Comparison logic: score DESC, index ASC
            if (s[j] > pivotS || (s[j] == pivotS && i[j] < pivotI)) {
                swap(s, i, j, storeIdx);
                storeIdx++;
            }
        }
        swap(s, i, storeIdx, right);
        return storeIdx;
    }

    private static void sortTopK(int[] s, int[] i, int k) {
        // Dual-pivot or standard sort on the first k elements
        // Since k is usually small, we can use a custom sort or boxed sort.
        // To keep it fast and primitive, we use a simple quicksort on the k elements.
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
        swap(s, i, mid, right);
        int pivotS = s[right];
        int pivotI = i[right];
        int storeIdx = left;
        for (int j = left; j < right; j++) {
            if (s[j] > pivotS || (s[j] == pivotS && i[j] < pivotI)) {
                swap(s, i, j, storeIdx);
                storeIdx++;
            }
        }
        swap(s, i, storeIdx, right);
        return storeIdx;
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