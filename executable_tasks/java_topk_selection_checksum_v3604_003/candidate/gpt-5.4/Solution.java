import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Max-heap over the "worst" element among current top-k at root.
  // Ordering of winners is: score desc, index asc.
  // Therefore "worse" means smaller score, or same score with larger index.
  private static final class TopKHeap {
    final int[] scores;
    final int[] indices;
    int size;

    TopKHeap(int k) {
      this.scores = new int[k];
      this.indices = new int[k];
      this.size = 0;
    }

    void clear() {
      size = 0;
    }

    // Returns true if (s1,i1) is worse than (s2,i2)
    static boolean worse(int s1, int i1, int s2, int i2) {
      return s1 < s2 || (s1 == s2 && i1 > i2);
    }

    // Returns true if (s1,i1) is better than (s2,i2)
    static boolean better(int s1, int i1, int s2, int i2) {
      return s1 > s2 || (s1 == s2 && i1 < i2);
    }

    void offer(int score, int index) {
      int pos = size;
      scores[pos] = score;
      indices[pos] = index;
      size = pos + 1;
      siftUp(pos);
    }

    void replaceRoot(int score, int index) {
      scores[0] = score;
      indices[0] = index;
      siftDown(0);
    }

    private void siftUp(int pos) {
      int s = scores[pos];
      int idx = indices[pos];
      while (pos > 0) {
        int parent = (pos - 1) >>> 1;
        int ps = scores[parent];
        int pi = indices[parent];
        if (!worse(s, idx, ps, pi)) break;
        scores[pos] = ps;
        indices[pos] = pi;
        pos = parent;
      }
      scores[pos] = s;
      indices[pos] = idx;
    }

    private void siftDown(int pos) {
      int half = size >>> 1;
      int s = scores[pos];
      int idx = indices[pos];
      while (pos < half) {
        int left = (pos << 1) + 1;
        int right = left + 1;
        int child = left;
        int cs = scores[left];
        int ci = indices[left];
        if (right < size) {
          int rs = scores[right];
          int ri = indices[right];
          if (worse(rs, ri, cs, ci)) {
            child = right;
            cs = rs;
            ci = ri;
          }
        }
        if (!worse(cs, ci, s, idx)) break;
        scores[pos] = cs;
        indices[pos] = ci;
        pos = child;
      }
      scores[pos] = s;
      indices[pos] = idx;
    }
  }

  // Sort winners by required final order: score desc, index asc.
  private static void sortWinners(int[] scores, int[] indices, int n) {
    quickSort(scores, indices, 0, n - 1);
  }

  private static void quickSort(int[] scores, int[] indices, int left, int right) {
    while (left < right) {
      if (right - left <= 24) {
        insertionSort(scores, indices, left, right);
        return;
      }

      int mid = (left + right) >>> 1;
      int a = left, b = mid, c = right;
      if (comesBefore(scores[b], indices[b], scores[a], indices[a])) { int ts = scores[a]; scores[a] = scores[b]; scores[b] = ts; int ti = indices[a]; indices[a] = indices[b]; indices[b] = ti; }
      if (comesBefore(scores[c], indices[c], scores[b], indices[b])) { int ts = scores[b]; scores[b] = scores[c]; scores[c] = ts; int ti = indices[b]; indices[b] = indices[c]; indices[c] = ti; }
      if (comesBefore(scores[b], indices[b], scores[a], indices[a])) { int ts = scores[a]; scores[a] = scores[b]; scores[b] = ts; int ti = indices[a]; indices[a] = indices[b]; indices[b] = ti; }

      int pivotScore = scores[b];
      int pivotIndex = indices[b];

      int i = left, j = right;
      while (i <= j) {
        while (comesBefore(scores[i], indices[i], pivotScore, pivotIndex)) i++;
        while (comesBefore(pivotScore, pivotIndex, scores[j], indices[j])) j--;
        if (i <= j) {
          int ts = scores[i]; scores[i] = scores[j]; scores[j] = ts;
          int ti = indices[i]; indices[i] = indices[j]; indices[j] = ti;
          i++;
          j--;
        }
      }

      if (j - left < right - i) {
        if (left < j) quickSort(scores, indices, left, j);
        left = i;
      } else {
        if (i < right) quickSort(scores, indices, i, right);
        right = j;
      }
    }
  }

  private static void insertionSort(int[] scores, int[] indices, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
      int s = scores[i];
      int idx = indices[i];
      int j = i - 1;
      while (j >= left && comesBefore(s, idx, scores[j], indices[j])) {
        scores[j + 1] = scores[j];
        indices[j + 1] = indices[j];
        j--;
      }
      scores[j + 1] = s;
      indices[j + 1] = idx;
    }
  }

  // Required order: score desc, index asc.
  private static boolean comesBefore(int s1, int i1, int s2, int i2) {
    return s1 > s2 || (s1 == s2 && i1 < i2);
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) {
        h = FNV_OFFSET_BASIS;
      }
      return h;
    }
    if (k > n) k = n;

    TopKHeap heap = new TopKHeap(k);
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      heap.clear();

      int i = 0;
      for (; i < k; i++) {
        heap.offer(scores[i], i);
      }

      for (; i < n; i++) {
        int s = scores[i];
        int rootS = heap.scores[0];
        int rootI = heap.indices[0];
        if (TopKHeap.better(s, i, rootS, rootI)) {
          heap.replaceRoot(s, i);
        }
      }

      sortWinners(heap.scores, heap.indices, k);

      h = FNV_OFFSET_BASIS;
      for (i = 0; i < k; i++) {
        h ^= (((long) heap.scores[i]) << 32) ^ (heap.indices[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
