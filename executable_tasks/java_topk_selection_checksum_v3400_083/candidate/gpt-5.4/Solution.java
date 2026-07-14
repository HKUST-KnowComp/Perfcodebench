import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }
    if (k > n) k = n;

    long h = 0L;

    // Fast path: if k == n, avoid selection and sort all indices directly.
    if (k == n) {
      int[] idx = new int[n];
      for (int i = 0; i < n; i++) idx[i] = i;
      sortIndicesByScoreDescIndexAsc(idx, 0, n - 1, scores);
      for (int iter = 0; iter < iters; iter++) {
        h = FNV_OFFSET;
        for (int i = 0; i < n; i++) {
          int id = idx[i];
          h ^= (((long) scores[id]) << 32) ^ (id & 0xffffffffL);
          h *= FNV_PRIME;
        }
      }
      return h;
    }

    // Reusable buffers across iterations.
    int[] heapIdx = new int[k];
    int heapSize = 0;
    int[] winners = new int[k];

    for (int iter = 0; iter < iters; iter++) {
      heapSize = 0;

      // Maintain a min-heap of the current top-k according to the target order
      // (best = higher score, then lower index). Heap root is the worst among winners.
      for (int i = 0; i < n; i++) {
        if (heapSize < k) {
          heapIdx[heapSize] = i;
          siftUpMinHeap(heapIdx, heapSize, scores);
          heapSize++;
        } else {
          int root = heapIdx[0];
          int si = scores[i];
          int sr = scores[root];
          if (si > sr || (si == sr && i < root)) {
            heapIdx[0] = i;
            siftDownMinHeap(heapIdx, 0, heapSize, scores);
          }
        }
      }

      System.arraycopy(heapIdx, 0, winners, 0, k);
      sortIndicesByScoreDescIndexAsc(winners, 0, k - 1, scores);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        int id = winners[i];
        h ^= (((long) scores[id]) << 32) ^ (id & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }

    return h;
  }

  // Min-heap ordered by worse-first:
  // lower score is worse; for equal score, larger index is worse.
  private static void siftUpMinHeap(int[] heap, int pos, int[] scores) {
    int v = heap[pos];
    int sv = scores[v];
    while (pos > 0) {
      int parent = (pos - 1) >>> 1;
      int p = heap[parent];
      int sp = scores[p];
      if (sv > sp || (sv == sp && v < p)) {
        break;
      }
      heap[pos] = p;
      pos = parent;
    }
    heap[pos] = v;
  }

  private static void siftDownMinHeap(int[] heap, int pos, int size, int[] scores) {
    int v = heap[pos];
    int sv = scores[v];
    int half = size >>> 1;
    while (pos < half) {
      int left = (pos << 1) + 1;
      int right = left + 1;
      int child = left;
      int c = heap[left];
      int sc = scores[c];

      if (right < size) {
        int rc = heap[right];
        int src = scores[rc];
        if (src < sc || (src == sc && rc > c)) {
          child = right;
          c = rc;
          sc = src;
        }
      }

      if (sv < sc || (sv == sc && v > c)) {
        break;
      }
      heap[pos] = c;
      pos = child;
    }
    heap[pos] = v;
  }

  private static void sortIndicesByScoreDescIndexAsc(int[] a, int left, int right, int[] scores) {
    if (left >= right) return;
    quickSort(a, left, right, scores);
    insertionSort(a, left, right, scores);
  }

  private static void quickSort(int[] a, int left, int right, int[] scores) {
    while (right - left > 32) {
      int mid = (left + right) >>> 1;
      int pivot = medianOf3(a[left], a[mid], a[right], scores);
      int i = left;
      int j = right;
      while (i <= j) {
        while (better(a[i], pivot, scores)) i++;
        while (better(pivot, a[j], scores)) j--;
        if (i <= j) {
          int t = a[i];
          a[i++] = a[j];
          a[j--] = t;
        }
      }
      if (j - left < right - i) {
        if (left < j) quickSort(a, left, j, scores);
        left = i;
      } else {
        if (i < right) quickSort(a, i, right, scores);
        right = j;
      }
    }
  }

  private static void insertionSort(int[] a, int left, int right, int[] scores) {
    for (int i = left + 1; i <= right; i++) {
      int v = a[i];
      int j = i - 1;
      while (j >= left && better(v, a[j], scores)) {
        a[j + 1] = a[j];
        j--;
      }
      a[j + 1] = v;
    }
  }

  private static int medianOf3(int x, int y, int z, int[] scores) {
    if (better(x, y, scores)) {
      if (better(y, z, scores)) return y;
      return better(x, z, scores) ? z : x;
    } else {
      if (better(x, z, scores)) return x;
      return better(y, z, scores) ? z : y;
    }
  }

  // True if a should come before b in final ordering:
  // higher score first, then lower index first.
  private static boolean better(int a, int b, int[] scores) {
    int sa = scores[a];
    int sb = scores[b];
    return sa > sb || (sa == sb && a < b);
  }
}
