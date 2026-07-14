import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] idx = new int[n];
    int[] topIdx = new int[k];
    int[] topScores = new int[k];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) idx[i] = i;
      
      // Partial quickselect to get top-k indices
      quickselect(scores, idx, 0, n - 1, k);
      
      // Copy top-k to separate arrays for sorting
      for (int i = 0; i < k; i++) {
        topIdx[i] = idx[i];
        topScores[i] = scores[idx[i]];
      }
      
      // Sort only the top-k elements
      sortTopK(topScores, topIdx, k);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topScores[i]) << 32) ^ (topIdx[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickselect(int[] scores, int[] idx, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(scores, idx, left, right);
      if (pivotIndex == k - 1) {
        return;
      } else if (pivotIndex < k - 1) {
        left = pivotIndex + 1;
      } else {
        right = pivotIndex - 1;
      }
    }
  }
  
  private static int partition(int[] scores, int[] idx, int left, int right) {
    // Median-of-three pivot selection
    int mid = left + (right - left) / 2;
    if (compare(scores, idx, left, mid) < 0) swap(idx, left, mid);
    if (compare(scores, idx, left, right) < 0) swap(idx, left, right);
    if (compare(scores, idx, mid, right) < 0) swap(idx, mid, right);
    swap(idx, mid, right);
    
    int pivotIdx = idx[right];
    int pivotScore = scores[pivotIdx];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      int js = scores[idx[j]];
      // Descending by score, ascending by index for ties
      if (js > pivotScore || (js == pivotScore && idx[j] < pivotIdx)) {
        i++;
        swap(idx, i, j);
      }
    }
    swap(idx, i + 1, right);
    return i + 1;
  }
  
  private static int compare(int[] scores, int[] idx, int i, int j) {
    int si = scores[idx[i]];
    int sj = scores[idx[j]];
    if (si != sj) return Integer.compare(sj, si); // descending
    return Integer.compare(idx[i], idx[j]); // ascending by index
  }
  
  private static void swap(int[] arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
  }
  
  private static void sortTopK(int[] topScores, int[] topIdx, int k) {
    // Insertion sort for small k, otherwise use quicksort
    if (k <= 32) {
      for (int i = 1; i < k; i++) {
        int score = topScores[i];
        int index = topIdx[i];
        int j = i - 1;
        while (j >= 0 && (topScores[j] < score || (topScores[j] == score && topIdx[j] > index))) {
          topScores[j + 1] = topScores[j];
          topIdx[j + 1] = topIdx[j];
          j--;
        }
        topScores[j + 1] = score;
        topIdx[j + 1] = index;
      }
    } else {
      quicksortTopK(topScores, topIdx, 0, k - 1);
    }
  }
  
  private static void quicksortTopK(int[] topScores, int[] topIdx, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    int pivotScore = topScores[mid];
    int pivotIdx = topIdx[mid];
    int i = left, j = right;
    while (i <= j) {
      while (topScores[i] > pivotScore || (topScores[i] == pivotScore && topIdx[i] < pivotIdx)) i++;
      while (topScores[j] < pivotScore || (topScores[j] == pivotScore && topIdx[j] > pivotIdx)) j--;
      if (i <= j) {
        int ts = topScores[i]; topScores[i] = topScores[j]; topScores[j] = ts;
        int ti = topIdx[i]; topIdx[i] = topIdx[j]; topIdx[j] = ti;
        i++; j--;
      }
    }
    if (left < j) quicksortTopK(topScores, topIdx, left, j);
    if (i < right) quicksortTopK(topScores, topIdx, i, right);
  }
}