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
      sortTopK(scores, idx, k);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        int index = idx[i];
        int score = scores[index];
        h ^= (((long) score) << 32) ^ (index & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  // Quickselect to ensure top-k elements (by score desc, index asc) are in idx[0..k-1]
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
      int jIdx = idx[j];
      int jScore = scores[jIdx];
      // Compare: higher score first, then lower index first
      if (jScore > pivotScore || (jScore == pivotScore && jIdx < pivotIdx)) {
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
    if (si != sj) return Integer.compare(sj, si); // higher score first
    return Integer.compare(idx[i], idx[j]); // lower index first
  }
  
  private static void swap(int[] arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
  }
  
  // Insertion sort for small k, otherwise use Arrays.sort approach
  private static void sortTopK(int[] scores, int[] idx, int k) {
    if (k <= 32) {
      // Insertion sort for small k
      for (int i = 1; i < k; i++) {
        int key = idx[i];
        int keyScore = scores[key];
        int j = i - 1;
        while (j >= 0) {
          int jScore = scores[idx[j]];
          if (jScore > keyScore || (jScore == keyScore && idx[j] < key)) break;
          idx[j + 1] = idx[j];
          j--;
        }
        idx[j + 1] = key;
      }
    } else {
      // For larger k, use a simple quicksort on the first k elements
      quicksortRange(scores, idx, 0, k - 1);
    }
  }
  
  private static void quicksortRange(int[] scores, int[] idx, int left, int right) {
    if (left >= right) return;
    int pivotIndex = partitionSort(scores, idx, left, right);
    quicksortRange(scores, idx, left, pivotIndex - 1);
    quicksortRange(scores, idx, pivotIndex + 1, right);
  }
  
  private static int partitionSort(int[] scores, int[] idx, int left, int right) {
    int mid = left + (right - left) / 2;
    if (compare(scores, idx, left, mid) < 0) swap(idx, left, mid);
    if (compare(scores, idx, left, right) < 0) swap(idx, left, right);
    if (compare(scores, idx, mid, right) < 0) swap(idx, mid, right);
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