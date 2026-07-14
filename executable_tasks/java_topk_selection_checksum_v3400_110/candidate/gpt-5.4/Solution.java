import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Compare entries by desired final order:
  // higher score first, then lower index first.
  private static boolean better(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA > scoreB || (scoreA == scoreB && indexA < indexB);
  }

  // Compare entries by heap order for a min-heap of current winners:
  // root is the worst among the kept top-k.
  private static boolean heapLess(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA < scoreB || (scoreA == scoreB && indexA > indexB);
  }

  private static void siftDown(int[] heapScores, int[] heapIdx, int size, int pos) {
    int s = heapScores[pos];
    int idx = heapIdx[pos];
    int half = size >>> 1;
    while (pos < half) {
      int left = (pos << 1) + 1;
      int right = left + 1;
      int child = left;
      int cs = heapScores[left];
      int ci = heapIdx[left];
      if (right < size) {
        int rs = heapScores[right];
        int ri = heapIdx[right];
        if (heapLess(rs, ri, cs, ci)) {
          child = right;
          cs = rs;
          ci = ri;
        }
      }
      if (!heapLess(cs, ci, s, idx)) break;
      heapScores[pos] = cs;
      heapIdx[pos] = ci;
      pos = child;
    }
    heapScores[pos] = s;
    heapIdx[pos] = idx;
  }

  private static void siftUp(int[] heapScores, int[] heapIdx, int pos) {
    int s = heapScores[pos];
    int idx = heapIdx[pos];
    while (pos > 0) {
      int parent = (pos - 1) >>> 1;
      int ps = heapScores[parent];
      int pi = heapIdx[parent];
      if (!heapLess(s, idx, ps, pi)) break;
      heapScores[pos] = ps;
      heapIdx[pos] = pi;
      pos = parent;
    }
    heapScores[pos] = s;
    heapIdx[pos] = idx;
  }

  private static void sortTopK(int[] scores, int[] idx, int n) {
    for (int i = 1; i < n; i++) {
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

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }
    if (k > n) k = n;

    int[] heapScores = new int[k];
    int[] heapIdx = new int[k];
    int[] topScores = new int[k];
    int[] topIdx = new int[k];

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < k; i++) {
        heapScores[i] = scores[i];
        heapIdx[i] = i;
      }

      for (int i = (k >>> 1) - 1; i >= 0; i--) {
        siftDown(heapScores, heapIdx, k, i);
      }

      for (int i = k; i < n; i++) {
        int s = scores[i];
        int idx = i;
        if (better(s, idx, heapScores[0], heapIdx[0])) {
          heapScores[0] = s;
          heapIdx[0] = idx;
          siftDown(heapScores, heapIdx, k, 0);
        }
      }

      System.arraycopy(heapScores, 0, topScores, 0, k);
      System.arraycopy(heapIdx, 0, topIdx, 0, k);
      sortTopK(topScores, topIdx, k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topScores[i]) << 32) ^ (topIdx[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
