import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      int[] indices = new int[scores.length];
      for (int i = 0; i < scores.length; i++) indices[i] = i;
      quickSelect(scores, indices, k);
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) scores[indices[i]]) << 32) ^ (indices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  private static void quickSelect(int[] scores, int[] indices, int k) {
    int left = 0, right = indices.length - 1;
    Random random = new Random();
    while (left < right) {
      int pivotIndex = partition(scores, indices, left, right, random);
      if (pivotIndex == k - 1) break;
      if (pivotIndex < k - 1) left = pivotIndex + 1;
      else right = pivotIndex - 1;
    }
  }

  private static int partition(int[] scores, int[] indices, int left, int right, Random random) {
    int pivotIndex = left + random.nextInt(right - left + 1);
    swap(indices, pivotIndex, right);
    int pivot = scores[indices[right]];
    int i = left - 1;
    for (int j = left; j < right; j++) {
      if (scores[indices[j]] > pivot || (scores[indices[j]] == pivot && indices[j] < indices[right])) {
        i++;
        swap(indices, i, j);
      }
    }
    swap(indices, i + 1, right);
    return i + 1;
  }

  private static void swap(int[] arr, int i, int j) {
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}