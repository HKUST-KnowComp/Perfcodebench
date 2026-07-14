import java.util.*;

public class Solution {
  static class Pair {
    int score;
    int index;
    Pair(int score, int index) { this.score = score; this.index = index; }
  }

  public static long run(int[] scores, int k, int iters) {
    Pair[] pairs = new Pair[scores.length];
    long h = 0;
    Random rand = new Random();
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < scores.length; i++) pairs[i] = new Pair(scores[i], i);
      quickSelect(pairs, k, rand);
      Arrays.sort(pairs, 0, k, (a, b) -> {
        if (a.score != b.score) return Integer.compare(b.score, a.score);
        return Integer.compare(a.index, b.index);
      });
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) pairs[i].score) << 32) ^ (pairs[i].index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(Pair[] pairs, int k, Random rand) {
    int left = 0, right = pairs.length - 1;
    while (left < right) {
      int pivotIndex = partition(pairs, left, right, rand);
      if (pivotIndex == k - 1) break;
      if (pivotIndex < k - 1) left = pivotIndex + 1;
      else right = pivotIndex - 1;
    }
  }

  private static int partition(Pair[] pairs, int left, int right, Random rand) {
    int pivotIndex = rand.nextInt(right - left + 1) + left;
    Pair pivot = pairs[pivotIndex];
    swap(pairs, pivotIndex, right);
    int storeIndex = left;
    for (int i = left; i < right; i++) {
      if (compare(pairs[i], pivot) > 0) {
        swap(pairs, storeIndex, i);
        storeIndex++;
      }
    }
    swap(pairs, right, storeIndex);
    return storeIndex;
  }

  private static int compare(Pair a, Pair b) {
    if (a.score != b.score) return Integer.compare(b.score, a.score);
    return Integer.compare(a.index, b.index);
  }

  private static void swap(Pair[] pairs, int i, int j) {
    Pair temp = pairs[i];
    pairs[i] = pairs[j];
    pairs[j] = temp;
  }
}