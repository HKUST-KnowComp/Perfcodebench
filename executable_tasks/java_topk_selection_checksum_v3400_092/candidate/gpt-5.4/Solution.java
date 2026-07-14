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

    long[] top = new long[k];
    long h = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;

      for (int i = 0; i < n; i++) {
        long key = pack(scores[i], i);
        if (size < k) {
          top[size] = key;
          siftUpWorstHeap(top, size);
          size++;
        } else if (Long.compare(key, top[0]) < 0) {
          top[0] = key;
          siftDownWorstHeap(top, 0, k);
        }
      }

      Arrays.sort(top, 0, k);

      h = FNV_OFFSET;
      for (int i = 0; i < k; i++) {
        long key = top[i];
        int score = unpackScore(key);
        int index = unpackIndex(key);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }

  private static long pack(int score, int index) {
    return (((long) ~score) << 32) | (index & 0xffffffffL);
  }

  private static int unpackScore(long key) {
    return ~((int) (key >>> 32));
  }

  private static int unpackIndex(long key) {
    return (int) key;
  }

  private static void siftUpWorstHeap(long[] heap, int idx) {
    long v = heap[idx];
    while (idx > 0) {
      int parent = (idx - 1) >>> 1;
      long p = heap[parent];
      if (Long.compare(v, p) <= 0) break;
      heap[idx] = p;
      idx = parent;
    }
    heap[idx] = v;
  }

  private static void siftDownWorstHeap(long[] heap, int idx, int size) {
    long v = heap[idx];
    int half = size >>> 1;
    while (idx < half) {
      int left = (idx << 1) + 1;
      int right = left + 1;
      int child = left;
      long c = heap[left];
      if (right < size && Long.compare(heap[right], c) > 0) {
        child = right;
        c = heap[right];
      }
      if (Long.compare(v, c) >= 0) break;
      heap[idx] = c;
      idx = child;
    }
    heap[idx] = v;
  }
}
