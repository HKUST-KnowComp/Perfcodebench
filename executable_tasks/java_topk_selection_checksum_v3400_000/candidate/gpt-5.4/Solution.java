import java.util.*;

public class Solution {
  // Pack (score, index) into a single long so natural ascending order on the key
  // corresponds to desired ranking: higher score first, then lower index first.
  // key = ((~score) << 32) | indexUnsigned
  // Smaller key => better rank.

  private static long makeKey(int score, int index) {
    return (((long) ~score) << 32) | (index & 0xffffffffL);
  }

  private static int compareKey(long a, long b) {
    return Long.compare(a, b);
  }

  private static void siftDown(long[] heap, int size, int pos) {
    while (true) {
      int left = (pos << 1) + 1;
      if (left >= size) return;
      int right = left + 1;
      int largest = left;
      if (right < size && compareKey(heap[left], heap[right]) < 0) {
        largest = right;
      }
      if (compareKey(heap[pos], heap[largest]) >= 0) return;
      long tmp = heap[pos];
      heap[pos] = heap[largest];
      heap[largest] = tmp;
      pos = largest;
    }
  }

  private static void siftUp(long[] heap, int pos) {
    while (pos > 0) {
      int parent = (pos - 1) >>> 1;
      if (compareKey(heap[parent], heap[pos]) >= 0) return;
      long tmp = heap[parent];
      heap[parent] = heap[pos];
      heap[pos] = tmp;
      pos = parent;
    }
  }

  private static void sortTopK(long[] a, int n) {
    // Insertion sort is typically excellent for small/medium k.
    for (int i = 1; i < n; i++) {
      long v = a[i];
      int j = i - 1;
      while (j >= 0 && compareKey(a[j], v) > 0) {
        a[j + 1] = a[j];
        j--;
      }
      a[j + 1] = v;
    }
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    long h = 0L;

    if (k <= 0) {
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }

    if (k >= n) {
      long[] all = new long[n];
      for (int iter = 0; iter < iters; iter++) {
        for (int i = 0; i < n; i++) {
          all[i] = makeKey(scores[i], i);
        }
        Arrays.sort(all);
        h = 1469598103934665603L;
        for (int i = 0; i < n; i++) {
          long key = all[i];
          int score = ~((int) (key >>> 32));
          int index = (int) key;
          h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
          h *= 1099511628211L;
        }
      }
      return h;
    }

    long[] heap = new long[k];
    for (int iter = 0; iter < iters; iter++) {
      int size = 0;
      for (int i = 0; i < n; i++) {
        long key = makeKey(scores[i], i);
        if (size < k) {
          heap[size] = key;
          siftUp(heap, size);
          size++;
        } else if (compareKey(key, heap[0]) < 0) {
          heap[0] = key;
          siftDown(heap, k, 0);
        }
      }

      sortTopK(heap, k);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        long key = heap[i];
        int score = ~((int) (key >>> 32));
        int index = (int) key;
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
