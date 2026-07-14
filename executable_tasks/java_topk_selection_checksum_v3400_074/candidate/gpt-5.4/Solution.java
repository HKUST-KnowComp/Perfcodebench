import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Max-heap over the current top-k winners where the root is the worst winner.
  // Ordering of winners: better = higher score, then lower index.
  // Heap root keeps the opposite: worse = lower score, then higher index.
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

    // Returns true if candidate is better than heap element at pos.
    private boolean betterThan(int score, int index, int pos) {
      int ps = scores[pos];
      if (score != ps) return score > ps;
      return index < indices[pos];
    }

    // Returns true if element a is worse than element b.
    private boolean worse(int as, int ai, int bs, int bi) {
      if (as != bs) return as < bs;
      return ai > bi;
    }

    void offer(int score, int index) {
      int s = size;
      if (s < scores.length) {
        scores[s] = score;
        indices[s] = index;
        size = s + 1;
        siftUp(s);
      } else if (s != 0 && betterThan(score, index, 0)) {
        scores[0] = score;
        indices[0] = index;
        siftDown(0);
      }
    }

    private void siftUp(int i) {
      int score = scores[i];
      int index = indices[i];
      while (i > 0) {
        int p = (i - 1) >>> 1;
        int ps = scores[p];
        int pi = indices[p];
        if (!worse(score, index, ps, pi)) break;
        scores[i] = ps;
        indices[i] = pi;
        i = p;
      }
      scores[i] = score;
      indices[i] = index;
    }

    private void siftDown(int i) {
      int score = scores[i];
      int index = indices[i];
      int half = size >>> 1;
      while (i < half) {
        int left = (i << 1) + 1;
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
        if (!worse(cs, ci, score, index)) break;
        scores[i] = cs;
        indices[i] = ci;
        i = child;
      }
      scores[i] = score;
      indices[i] = index;
    }
  }

  // Sort winners into required final order: score desc, index asc.
  private static void sortWinners(int[] scores, int[] indices, int n) {
    for (int i = 1; i < n; i++) {
      int s = scores[i];
      int idx = indices[i];
      int j = i - 1;
      while (j >= 0) {
        int sj = scores[j];
        if (sj > s) break;
        if (sj == s && indices[j] < idx) break;
        scores[j + 1] = sj;
        indices[j + 1] = indices[j];
        j--;
      }
      scores[j + 1] = s;
      indices[j + 1] = idx;
    }
  }

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    if (k <= 0) {
      long h = 0L;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }
    if (k > n) k = n;

    TopKHeap heap = new TopKHeap(k);
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      heap.clear();
      for (int i = 0; i < n; i++) {
        heap.offer(scores[i], i);
      }

      sortWinners(heap.scores, heap.indices, k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        h ^= (((long) heap.scores[i]) << 32) ^ (heap.indices[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
