import java.util.*;

public class Solution {
  // Exact ordering required by the harness:
  // higher score first, and for equal score lower index first.
  // We avoid object allocation and full sorting of all items.

  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Min-heap of the current top-k winners, where the root is the worst winner.
  // "Worse" means lower score, or for equal score larger index.
  private static int[] heapScores;
  private static int[] heapIdx;
  private static int heapCap;

  // Scratch arrays for final ordering of the k winners.
  private static int[] outScores;
  private static int[] outIdx;

  private static void ensureCapacity(int k) {
    if (heapCap >= k) return;
    heapScores = new int[k];
    heapIdx = new int[k];
    outScores = new int[k];
    outIdx = new int[k];
    heapCap = k;
  }

  // Returns true if candidate (s1,i1) is better than (s2,i2)
  // under final ranking: higher score first, then lower index first.
  private static boolean better(int s1, int i1, int s2, int i2) {
    return s1 > s2 || (s1 == s2 && i1 < i2);
  }

  // Returns true if (s1,i1) is worse than (s2,i2)
  // under final ranking.
  private static boolean worse(int s1, int i1, int s2, int i2) {
    return s1 < s2 || (s1 == s2 && i1 > i2);
  }

  private static void siftUp(int pos) {
    int s = heapScores[pos];
    int idx = heapIdx[pos];
    while (pos > 0) {
      int parent = (pos - 1) >>> 1;
      int ps = heapScores[parent];
      int pi = heapIdx[parent];
      // Min-heap by "worse first".
      if (!worse(s, idx, ps, pi)) break;
      heapScores[pos] = ps;
      heapIdx[pos] = pi;
      pos = parent;
    }
    heapScores[pos] = s;
    heapIdx[pos] = idx;
  }

  private static void siftDown(int pos, int size) {
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

  // Sort winners into exact required order: better first.
  // Uses an iterative quicksort with insertion sort for small partitions.
  private static void sortTopK(int n) {
    if (n <= 1) return;

    int[] lstack = new int[64];
    int[] rstack = new int[64];
    int sp = 0;
    lstack[sp] = 0;
    rstack[sp] = n - 1;
    sp++;

    while (sp > 0) {
      sp--;
      int left = lstack[sp];
      int right = rstack[sp];

      while (right - left > 24) {
        int mid = (left + right) >>> 1;

        int aS = outScores[left], aI = outIdx[left];
        int bS = outScores[mid], bI = outIdx[mid];
        int cS = outScores[right], cI = outIdx[right];

        // median-of-three by final order (better first)
        int pS, pI;
        if (better(aS, aI, bS, bI)) {
          if (better(bS, bI, cS, cI)) {
            pS = bS; pI = bI;
          } else if (better(aS, aI, cS, cI)) {
            pS = cS; pI = cI;
          } else {
            pS = aS; pI = aI;
          }
        } else {
          if (better(aS, aI, cS, cI)) {
            pS = aS; pI = aI;
          } else if (better(bS, bI, cS, cI)) {
            pS = cS; pI = cI;
          } else {
            pS = bS; pI = bI;
          }
        }

        int i = left;
        int j = right;
        while (i <= j) {
          while (better(outScores[i], outIdx[i], pS, pI)) i++;
          while (better(pS, pI, outScores[j], outIdx[j])) j--;
          if (i <= j) {
            int ts = outScores[i]; outScores[i] = outScores[j]; outScores[j] = ts;
            int ti = outIdx[i]; outIdx[i] = outIdx[j]; outIdx[j] = ti;
            i++;
            j--;
          }
        }

        if (j - left < right - i) {
          if (i < right) {
            lstack[sp] = i;
            rstack[sp] = right;
            sp++;
          }
          right = j;
        } else {
          if (left < j) {
            lstack[sp] = left;
            rstack[sp] = j;
            sp++;
          }
          left = i;
        }
      }

      for (int i = left + 1; i <= right; i++) {
        int s = outScores[i];
        int idx = outIdx[i];
        int j = i - 1;
        while (j >= left && better(s, idx, outScores[j], outIdx[j])) {
          outScores[j + 1] = outScores[j];
          outIdx[j + 1] = outIdx[j];
          j--;
        }
        outScores[j + 1] = s;
        outIdx[j + 1] = idx;
      }
    }
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }

    ensureCapacity(k);
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;

      // Build/maintain exact top-k in O(n log k).
      for (int i = 0; i < n; i++) {
        int s = scores[i];
        if (size < k) {
          heapScores[size] = s;
          heapIdx[size] = i;
          siftUp(size);
          size++;
        } else {
          // Replace root only if candidate is better than current worst winner.
          if (better(s, i, heapScores[0], heapIdx[0])) {
            heapScores[0] = s;
            heapIdx[0] = i;
            siftDown(0, size);
          }
        }
      }

      // Copy winners to output arrays and sort them into exact final order.
      for (int i = 0; i < k; i++) {
        outScores[i] = heapScores[i];
        outIdx[i] = heapIdx[i];
      }
      sortTopK(k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        h ^= (((long) outScores[i]) << 32) ^ (outIdx[i] & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}
