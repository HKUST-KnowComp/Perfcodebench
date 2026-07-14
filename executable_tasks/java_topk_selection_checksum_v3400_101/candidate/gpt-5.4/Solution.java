import java.util.*;

public class Solution {
  // Exact ordering required by the task:
  // higher score first, and for equal score lower index first.
  private static boolean better(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA > scoreB || (scoreA == scoreB && indexA < indexB);
  }

  private static void swap(int[] scores, int[] idx, int a, int b) {
    int ts = scores[a];
    scores[a] = scores[b];
    scores[b] = ts;
    int ti = idx[a];
    idx[a] = idx[b];
    idx[b] = ti;
  }

  // Median-of-three pivot selection on the task comparator.
  private static int medianOf3(int[] scores, int[] idx, int a, int b, int c) {
    int sa = scores[a], ia = idx[a];
    int sb = scores[b], ib = idx[b];
    int sc = scores[c], ic = idx[c];

    if (better(sa, ia, sb, ib)) {
      if (better(sb, ib, sc, ic)) return b;
      return better(sa, ia, sc, ic) ? c : a;
    } else {
      if (better(sa, ia, sc, ic)) return a;
      return better(sb, ib, sc, ic) ? c : b;
    }
  }

  // 3-way partition quickselect so that positions [0, k) contain the exact top-k set.
  private static void selectTopK(int[] scores, int[] idx, int n, int k) {
    int left = 0, right = n - 1;
    int target = k - 1;

    while (left < right) {
      int mid = (left + right) >>> 1;
      int p = medianOf3(scores, idx, left, mid, right);
      int pivotScore = scores[p];
      int pivotIdx = idx[p];

      int lt = left;
      int i = left;
      int gt = right;

      while (i <= gt) {
        int si = scores[i];
        int ii = idx[i];
        if (better(si, ii, pivotScore, pivotIdx)) {
          swap(scores, idx, lt++, i++);
        } else if (better(pivotScore, pivotIdx, si, ii)) {
          swap(scores, idx, i, gt--);
        } else {
          i++;
        }
      }

      if (target < lt) {
        right = lt - 1;
      } else if (target > gt) {
        left = gt + 1;
      } else {
        return;
      }
    }
  }

  // Insertion sort is very fast for small k and nearly-partitioned data.
  private static void insertionSortTopK(int[] scores, int[] idx, int k) {
    for (int i = 1; i < k; i++) {
      int s = scores[i];
      int id = idx[i];
      int j = i - 1;
      while (j >= 0 && better(s, id, scores[j], idx[j])) {
        scores[j + 1] = scores[j];
        idx[j + 1] = idx[j];
        j--;
      }
      scores[j + 1] = s;
      idx[j + 1] = id;
    }
  }

  private static void heapSortTopK(int[] scores, int[] idx, int k) {
    // Build max-heap under the same comparator, then sort descending in-place.
    for (int i = (k >>> 1) - 1; i >= 0; i--) siftDown(scores, idx, i, k);
    for (int end = k - 1; end > 0; end--) {
      swap(scores, idx, 0, end);
      siftDown(scores, idx, 0, end);
    }
    // Heap sort above yields ascending under the comparator; reverse to descending.
    for (int i = 0, j = k - 1; i < j; i++, j--) swap(scores, idx, i, j);
  }

  private static void siftDown(int[] scores, int[] idx, int root, int size) {
    while (true) {
      int child = (root << 1) + 1;
      if (child >= size) return;
      int best = child;
      int right = child + 1;
      if (right < size && better(scores[right], idx[right], scores[child], idx[child])) {
        best = right;
      }
      if (!better(scores[best], idx[best], scores[root], idx[root])) return;
      swap(scores, idx, root, best);
      root = best;
    }
  }

  public static long run(int[] inputScores, int k, int iters) {
    final int n = inputScores.length;
    if (k <= 0) {
      long h = 1469598103934665603L;
      return h;
    }

    int[] scores = new int[n];
    int[] idx = new int[n];
    for (int i = 0; i < n; i++) idx[i] = i;

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      System.arraycopy(inputScores, 0, scores, 0, n);
      // idx is deterministic; refill to restore after prior iteration mutations.
      for (int i = 0; i < n; i++) idx[i] = i;

      if (k < n) {
        selectTopK(scores, idx, n, k);
      }

      if (k <= 64) {
        insertionSortTopK(scores, idx, k);
      } else {
        heapSortTopK(scores, idx, k);
      }

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) scores[i]) << 32) ^ (idx[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
