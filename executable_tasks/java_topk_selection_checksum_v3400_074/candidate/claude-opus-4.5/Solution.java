import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] idx = new int[n];
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) idx[i] = i;
      
      // Partial quickselect to partition so that top-k are in idx[0..k-1]
      quickselect(scores, idx, 0, n - 1, k);
      
      // Sort only the top-k elements
      sortTopK(scores, idx, 0, k - 1);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int index = idx[i];
        h ^= (((long) scores[index]) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }

  // Quickselect to ensure top-k elements are in idx[0..k-1]
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

  // Partition with descending score order, ascending index for ties
  private static int partition(int[] scores, int[] idx, int left, int right) {
    int mid = left + (right - left) / 2;
    // Median-of-three pivot selection
    if (compare(scores, idx, left, mid) > 0) swap(idx, left, mid);
    if (compare(scores, idx, left, right) > 0) swap(idx, left, right);
    if (compare(scores, idx, mid, right) > 0) swap(idx, mid, right);
    swap(idx, mid, right);
    
    int pivotIdx = idx[right];
    int pivotScore = scores[pivotIdx];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      int jIdx = idx[j];
      int jScore = scores[jIdx];
      // We want descending by score, ascending by index
      // So "less than" in our ordering means: higher score, or same score with lower index
      if (jScore > pivotScore || (jScore == pivotScore && jIdx < pivotIdx)) {
        i++;
        swap(idx, i, j);
      }
    }
    swap(idx, i + 1, right);
    return i + 1;
  }

  // Compare for descending score, ascending index
  private static int compare(int[] scores, int[] idx, int i, int j) {
    int si = scores[idx[i]], sj = scores[idx[j]];
    if (si != sj) return Integer.compare(sj, si); // descending score
    return Integer.compare(idx[i], idx[j]); // ascending index
  }

  private static void swap(int[] arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
  }

  // Insertion sort for small k, otherwise quicksort
  private static void sortTopK(int[] scores, int[] idx, int left, int right) {
    int len = right - left + 1;
    if (len <= 32) {
      // Insertion sort
      for (int i = left + 1; i <= right; i++) {
        int key = idx[i];
        int keyScore = scores[key];
        int j = i - 1;
        while (j >= left) {
          int jIdx = idx[j];
          int jScore = scores[jIdx];
          if (jScore > keyScore || (jScore == keyScore && jIdx < key)) break;
          idx[j + 1] = idx[j];
          j--;
        }
        idx[j + 1] = key;
      }
    } else {
      quicksortTopK(scores, idx, left, right);
    }
  }

  private static void quicksortTopK(int[] scores, int[] idx, int left, int right) {
    if (left >= right) return;
    int p = partitionSort(scores, idx, left, right);
    quicksortTopK(scores, idx, left, p - 1);
    quicksortTopK(scores, idx, p + 1, right);
  }

  private static int partitionSort(int[] scores, int[] idx, int left, int right) {
    int mid = left + (right - left) / 2;
    if (compare(scores, idx, left, mid) > 0) swap(idx, left, mid);
    if (compare(scores, idx, left, right) > 0) swap(idx, left, right);
    if (compare(scores, idx, mid, right) > 0) swap(idx, mid, right);
    swap(idx, mid, right);
    
    int pivotIdx = idx[right];
    int pivotScore = scores[pivotIdx];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      int jIdx = idx[j];
      int jScore = scores[jIdx];
      if (jScore > pivotScore || (jScore == pivotScore && jIdx < pivotIdx)) {
        i++;
        swap(idx, i, j);
      }
    }
    swap(idx, i + 1, right);
    return i + 1;
  }
}