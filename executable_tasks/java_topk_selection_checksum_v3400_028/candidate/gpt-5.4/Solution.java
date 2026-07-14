import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET_BASIS;
      return h;
    }
    if (k > n) k = n;

    final long[] keys = new long[n];
    final long[] top = new long[k];
    for (int i = 0; i < n; i++) {
      keys[i] = pack(scores[i], i);
    }

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      System.arraycopy(keys, 0, top, 0, k);
      heapifyMin(top, k);

      for (int i = k; i < n; i++) {
        long v = keys[i];
        if (Long.compareUnsigned(v, top[0]) > 0) {
          top[0] = v;
          siftDownMin(top, 0, k);
        }
      }

      Arrays.sort(top);

      h = FNV_OFFSET_BASIS;
      for (int i = k - 1; i >= 0; i--) {
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
    return (((long) score) << 32) | (0xffffffffL - (index & 0xffffffffL));
  }

  private static int unpackScore(long key) {
    return (int) (key >> 32);
  }

  private static int unpackIndex(long key) {
    return (int) (0xffffffffL - (key & 0xffffffffL));
  }

  private static void heapifyMin(long[] a, int size) {
    for (int i = (size >>> 1) - 1; i >= 0; i--) {
      siftDownMin(a, i, size);
    }
  }

  private static void siftDownMin(long[] a, int i, int size) {
    while (true) {
      int left = (i << 1) + 1;
      if (left >= size) return;
      int right = left + 1;
      int smallest = left;
      if (right < size && Long.compareUnsigned(a[right], a[left]) < 0) {
        smallest = right;
      }
      if (Long.compareUnsigned(a[i], a[smallest]) <= 0) return;
      long tmp = a[i];
      a[i] = a[smallest];
      a[smallest] = tmp;
      i = smallest;
    }
  }
}
