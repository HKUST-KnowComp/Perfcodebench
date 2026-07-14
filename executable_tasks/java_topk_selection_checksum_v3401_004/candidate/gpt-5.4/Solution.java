import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0L;
      for (int iter = 0; iter < iters; iter++) h = FNV_OFFSET_BASIS;
      return h;
    }
    if (k > n) k = n;

    final long[] keys = new long[n];
    for (int i = 0; i < n; i++) {
      keys[i] = makeKey(scores[i], i);
    }

    long h = 0L;
    for (int iter = 0; iter < iters; iter++) {
      final long threshold = selectKth(keys.clone(), k - 1);

      int greaterCount = 0;
      for (int i = 0; i < n; i++) {
        if (Long.compareUnsigned(keys[i], threshold) > 0) greaterCount++;
      }

      final int needEq = k - greaterCount;
      final long[] top = new long[k];
      int p = 0;
      for (int i = 0; i < n; i++) {
        if (Long.compareUnsigned(keys[i], threshold) > 0) top[p++] = keys[i];
      }
      if (needEq > 0) {
        for (int i = 0; i < n && p < k; i++) {
          if (keys[i] == threshold) top[p++] = keys[i];
        }
      }

      Arrays.sort(top);

      h = FNV_OFFSET_BASIS;
      for (int i = k - 1; i >= 0; i--) {
        long key = top[i];
        int score = decodeScore(key);
        int index = decodeIndex(key);
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= FNV_PRIME;
      }
    }
    return h;
  }

  private static long makeKey(int score, int index) {
    return ((((long) score) ^ 0x80000000L) << 32) | ((~index) & 0xffffffffL);
  }

  private static int decodeScore(long key) {
    return (int) ((key >>> 32) ^ 0x80000000L);
  }

  private static int decodeIndex(long key) {
    return ~((int) key);
  }

  private static long selectKth(long[] a, int k) {
    int left = 0;
    int right = a.length - 1;
    while (true) {
      if (left == right) return a[left];

      int mid = (left + right) >>> 1;
      long x = a[left];
      long y = a[mid];
      long z = a[right];
      long pivot = median3(x, y, z);

      int lt = left;
      int i = left;
      int gt = right;
      while (i <= gt) {
        int cmp = Long.compareUnsigned(a[i], pivot);
        if (cmp > 0) {
          swap(a, lt++, i++);
        } else if (cmp < 0) {
          swap(a, i, gt--);
        } else {
          i++;
        }
      }

      if (k < lt) {
        right = lt - 1;
      } else if (k > gt) {
        left = gt + 1;
      } else {
        return pivot;
      }
    }
  }

  private static long median3(long a, long b, long c) {
    if (Long.compareUnsigned(a, b) < 0) {
      long t = a; a = b; b = t;
    }
    if (Long.compareUnsigned(b, c) < 0) {
      long t = b; b = c; c = t;
    }
    if (Long.compareUnsigned(a, b) < 0) {
      long t = a; a = b; b = t;
    }
    return b;
  }

  private static void swap(long[] a, int i, int j) {
    long t = a[i];
    a[i] = a[j];
    a[j] = t;
  }
}
