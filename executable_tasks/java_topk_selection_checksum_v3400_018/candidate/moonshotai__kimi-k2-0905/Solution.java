import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    Pair[] pairs = new Pair[n];
    for (int i = 0; i < n; i++) pairs[i] = new Pair(scores[i], i);

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // quickselect to partition top-k into first k positions
      int lo = 0, hi = n - 1;
      while (true) {
        int pivot = partition(pairs, lo, hi);
        if (pivot == k - 1) break;
        else if (pivot < k - 1) lo = pivot + 1;
        else hi = pivot - 1;
      }
      // sort only the first k items to establish exact order
      Arrays.sort(pairs, 0, k, (a, b) -> {
        if (a.score != b.score) return Integer.compare(b.score, a.score);
        return Integer.compare(a.index, b.index);
      });
      // compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) pairs[i].score) << 32) ^ (pairs[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  // partition for quickselect: returns final index of pivot
  private static int partition(Pair[] a, int lo, int hi) {
    Pair pivot = a[hi];
    int i = lo;
    for (int j = lo; j < hi; j++) {
      if (compare(a[j], pivot) < 0) {
        Pair tmp = a[i]; a[i] = a[j]; a[j] = tmp;
        i++;
      }
    }
    Pair tmp = a[i]; a[i] = a[hi]; a[hi] = tmp;
    return i;
  }

  // comparator: higher score wins; on tie, smaller index wins
  private static int compare(Pair a, Pair b) {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  }
}