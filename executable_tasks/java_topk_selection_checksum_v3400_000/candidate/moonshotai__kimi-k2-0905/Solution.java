import java.util.*;

public class Solution {
    static class Pair {
        int score;
        int index;
        Pair(int score, int index) {
            this.score = score;
            this.index = index;
        }
    }

    public static long run(int[] scores, int k, int iters) {
        int n = scores.length;
        Pair[] pairs = new Pair[n];
        for (int i = 0; i < n; i++) pairs[i] = new Pair(scores[i], i);

        long h = 0;
        for (int iter = 0; iter < iters; iter++) {
            // Reset indices for this iteration
            for (int i = 0; i < n; i++) pairs[i].index = i;

            // Quickselect to partition top-k
            int low = 0, high = n - 1;
            while (true) {
                int pivot = partition(pairs, low, high);
                if (pivot == k - 1) break;
                else if (pivot < k - 1) low = pivot + 1;
                else high = pivot - 1;
            }

            // Sort the top-k slice to establish exact order
            Arrays.sort(pairs, 0, k, (a, b) -> {
                if (a.score != b.score) return Integer.compare(b.score, a.score);
                return Integer.compare(a.index, b.index);
            });

            // Compute checksum
            h = 1469598103934665603L;
            for (int i = 0; i < k; i++) {
                h ^= (((long) pairs[i].score) << 32) ^ (pairs[i].index & 0xffffffffL);
                h *= 1099511628211L;
            }
        }
        return h;
    }

    private static int partition(Pair[] a, int low, int high) {
        Pair pivot = a[high];
        int i = low;
        for (int j = low; j < high; j++) {
            int c = Integer.compare(a[j].score, pivot.score);
            if (c > 0 || (c == 0 && a[j].index < pivot.index)) {
                swap(a, i, j);
                i++;
            }
        }
        swap(a, i, high);
        return i;
    }

    private static void swap(Pair[] a, int i, int j) {
        Pair tmp = a[i];
        a[i] = a[j];
        a[j] = tmp;
    }
}