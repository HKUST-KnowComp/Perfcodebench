import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Compare two items in winner order:
  // higher score first, and for equal score lower index first.
  private static boolean better(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA > scoreB || (scoreA == scoreB && indexA < indexB);
  }

  // Compare two items in heap order (worst among current winners at root):
  // lower score is worse, and for equal score higher index is worse.
  private static boolean worse(int scoreA, int indexA, int scoreB, int indexB) {
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
        if (worse(rs, ri, cs, ci)) {
          child = right;
          cs = rs;
          ci = ri;
        }
      }

      if (!worse(cs, ci, s, idx)) break;
      heapScores[pos] = cs;
      heapIdx[pos] = ci;
      pos = child;
    }
    heapScores[pos] = s;
    heapIdx[pos] = idx;
  }

  private static void buildHeap(int[] heapScores, int[] heapIdx, int size) {
    for (int i = (size >>> 1) - 1; i >= 0; i--) {
      siftDown(heapScores, heapIdx, size, i);
    }
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
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < k; i++) {
        heapScores[i] = scores[i];
        heapIdx[i] = i;
      }
      buildHeap(heapScores, heapIdx, k);

      for (int i = k; i < n; i++) {
        int s = scores[i];
        int rootS = heapScores[0];
        int rootI = heapIdx[0];
        if (better(s, i, rootS, rootI)) {
          heapScores[0] = s;
          heapIdx[0] = i;
          siftDown(heapScores, heapIdx, k, 0);
        }
      }

      sortTopK(heapScores, heapIdx, k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        h ^= (((long) heapScores[i]) << 32) ^ (heapIdx[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
