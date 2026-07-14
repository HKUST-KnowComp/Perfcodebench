import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Order: higher score first, then lower index first.
  private static boolean better(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA > scoreB || (scoreA == scoreB && indexA < indexB);
  }

  // Min-heap of current winners where root is the worst among the kept top-k.
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

  private static void siftUp(int[] heapScores, int[] heapIdx, int pos) {
    int s = heapScores[pos];
    int idx = heapIdx[pos];
    while (pos > 0) {
      int parent = (pos - 1) >>> 1;
      int ps = heapScores[parent];
      int pi = heapIdx[parent];
      if (!worse(s, idx, ps, pi)) break;
      heapScores[pos] = ps;
      heapIdx[pos] = pi;
      pos = parent;
    }
    heapScores[pos] = s;
    heapIdx[pos] = idx;
  }

  private static void sortTopKDescending(int[] scores, int[] idx, int n) {
    // In-place heapsort using a max-heap under final desired order (best at root).
    for (int i = (n >>> 1) - 1; i >= 0; i--) {
      int pos = i;
      int s = scores[pos];
      int id = idx[pos];
      int half = n >>> 1;
      while (pos < half) {
        int left = (pos << 1) + 1;
        int right = left + 1;
        int child = left;
        int cs = scores[left];
        int ci = idx[left];
        if (right < n) {
          int rs = scores[right];
          int ri = idx[right];
          if (better(rs, ri, cs, ci)) {
            child = right;
            cs = rs;
            ci = ri;
          }
        }
        if (!better(cs, ci, s, id)) break;
        scores[pos] = cs;
        idx[pos] = ci;
        pos = child;
      }
      scores[pos] = s;
      idx[pos] = id;
    }

    for (int end = n - 1; end > 0; end--) {
      int ts = scores[0]; scores[0] = scores[end]; scores[end] = ts;
      int ti = idx[0]; idx[0] = idx[end]; idx[end] = ti;

      int pos = 0;
      int s = scores[0];
      int id = idx[0];
      int half = end >>> 1;
      while (pos < half) {
        int left = (pos << 1) + 1;
        int right = left + 1;
        int child = left;
        int cs = scores[left];
        int ci = idx[left];
        if (right < end) {
          int rs = scores[right];
          int ri = idx[right];
          if (better(rs, ri, cs, ci)) {
            child = right;
            cs = rs;
            ci = ri;
          }
        }
        if (!better(cs, ci, s, id)) break;
        scores[pos] = cs;
        idx[pos] = ci;
        pos = child;
      }
      scores[pos] = s;
      idx[pos] = id;
    }

    // Heapsort above yields ascending order under desired comparator; reverse.
    for (int l = 0, r = n - 1; l < r; l++, r--) {
      int ts = scores[l]; scores[l] = scores[r]; scores[r] = ts;
      int ti = idx[l]; idx[l] = idx[r]; idx[r] = ti;
    }
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }

    final int kk = k > n ? n : k;
    int[] heapScores = new int[kk];
    int[] heapIdx = new int[kk];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;

      for (int i = 0; i < n; i++) {
        int s = scores[i];
        if (size < kk) {
          heapScores[size] = s;
          heapIdx[size] = i;
          siftUp(heapScores, heapIdx, size);
          size++;
        } else {
          int rootS = heapScores[0];
          int rootI = heapIdx[0];
          if (better(s, i, rootS, rootI)) {
            heapScores[0] = s;
            heapIdx[0] = i;
            siftDown(heapScores, heapIdx, kk, 0);
          }
        }
      }

      sortTopKDescending(heapScores, heapIdx, kk);

      h = FNV_OFFSET;
      for (int i = 0; i < kk; i++) {
        h ^= (((long) heapScores[i]) << 32) ^ (heapIdx[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
