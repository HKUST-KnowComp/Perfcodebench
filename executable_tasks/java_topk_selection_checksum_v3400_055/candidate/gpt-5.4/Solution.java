import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0L;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET;
      return h;
    }
    if (k > n) k = n;

    long[] top = new long[k];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;
      long root = 0L;

      for (int i = 0; i < n; i++) {
        long key = pack(scores[i], i);
        if (size < k) {
          top[size] = key;
          siftUpWorse(top, size);
          size++;
          if (size == k) root = top[0];
        } else if (Long.compareUnsigned(key, root) > 0) {
          top[0] = key;
          siftDownWorse(top, 0, k);
          root = top[0];
        }
      }

      heapSortAscendingUnsigned(top, k);

      h = FNV_OFFSET;
      for (int i = k - 1; i >= 0; i--) {
        long key = top[i];
        h ^= unpackHashValue(key);
        h *= FNV_PRIME;
      }
    }
    return h;
  }

  private static long pack(int score, int index) {
    return (((long) score) << 32) | (0xffffffffL - (index & 0xffffffffL));
  }

  private static long unpackHashValue(long key) {
    int score = (int) (key >> 32);
    int index = (int) (0xffffffffL - (key & 0xffffffffL));
    return (((long) score) << 32) ^ (index & 0xffffffffL);
  }

  private static void siftUpWorse(long[] heap, int idx) {
    long v = heap[idx];
    while (idx > 0) {
      int parent = (idx - 1) >>> 1;
      long p = heap[parent];
      if (Long.compareUnsigned(v, p) >= 0) break;
      heap[idx] = p;
      idx = parent;
    }
    heap[idx] = v;
  }

  private static void siftDownWorse(long[] heap, int idx, int size) {
    long v = heap[idx];
    int half = size >>> 1;
    while (idx < half) {
      int left = (idx << 1) + 1;
      int right = left + 1;
      int child = left;
      long c = heap[left];
      if (right < size && Long.compareUnsigned(heap[right], c) < 0) {
        child = right;
        c = heap[right];
      }
      if (Long.compareUnsigned(v, c) <= 0) break;
      heap[idx] = c;
      idx = child;
    }
    heap[idx] = v;
  }

  private static void heapSortAscendingUnsigned(long[] a, int size) {
    for (int end = size - 1; end > 0; end--) {
      long t = a[0];
      a[0] = a[end];
      a[end] = t;
      siftDownMaxUnsigned(a, 0, end);
    }
  }

  private static void siftDownMaxUnsigned(long[] heap, int idx, int size) {
    long v = heap[idx];
    int half = size >>> 1;
    while (idx < half) {
      int left = (idx << 1) + 1;
      int right = left + 1;
      int child = left;
      long c = heap[left];
      if (right < size && Long.compareUnsigned(heap[right], c) > 0) {
        child = right;
        c = heap[right];
      }
      if (Long.compareUnsigned(v, c) >= 0) break;
      heap[idx] = c;
      idx = child;
    }
    heap[idx] = v;
  }
}
