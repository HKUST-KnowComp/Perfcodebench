import java.util.*;

public class Solution {
  private static boolean better(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA > scoreB || (scoreA == scoreB && indexA < indexB);
  }

  private static boolean worse(int scoreA, int indexA, int scoreB, int indexB) {
    return scoreA < scoreB || (scoreA == scoreB && indexA > indexB);
  }

  private static void siftUp(int[] heap, int size, int[] scores, int idx) {
    int v = heap[idx];
    int sv = scores[v];
    while (idx > 0) {
      int p = (idx - 1) >>> 1;
      int hp = heap[p];
      int sp = scores[hp];
      if (!worse(sv, v, sp, hp)) break;
      heap[idx] = hp;
      idx = p;
    }
    heap[idx] = v;
  }

  private static void siftDown(int[] heap, int size, int[] scores, int idx) {
    int v = heap[idx];
    int sv = scores[v];
    int half = size >>> 1;
    while (idx < half) {
      int left = (idx << 1) + 1;
      int right = left + 1;
      int child = left;
      int hc = heap[left];
      int sc = scores[hc];
      if (right < size) {
        int hr = heap[right];
        int sr = scores[hr];
        if (worse(sr, hr, sc, hc)) {
          child = right;
          hc = hr;
          sc = sr;
        }
      }
      if (!worse(sc, hc, sv, v)) break;
      heap[idx] = hc;
      idx = child;
    }
    heap[idx] = v;
  }

  private static void sortTopK(int[] arr, int k, int[] scores) {
    for (int i = 1; i < k; i++) {
      int v = arr[i];
      int sv = scores[v];
      int j = i - 1;
      while (j >= 0 && better(sv, v, scores[arr[j]], arr[j])) {
        arr[j + 1] = arr[j];
        j--;
      }
      arr[j + 1] = v;
    }
  }

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0) {
      long h = 0;
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }

    final int[] heap = new int[k];
    long h = 0;

    for (int iter = 0; iter < iters; iter++) {
      int size = 0;
      for (int i = 0; i < n; i++) {
        if (size < k) {
          heap[size] = i;
          siftUp(heap, size + 1, scores, size);
          size++;
        } else {
          int root = heap[0];
          if (better(scores[i], i, scores[root], root)) {
            heap[0] = i;
            siftDown(heap, k, scores, 0);
          }
        }
      }

      sortTopK(heap, k, scores);

      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int idx = heap[i];
        h ^= (((long) scores[idx]) << 32) ^ (idx & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
