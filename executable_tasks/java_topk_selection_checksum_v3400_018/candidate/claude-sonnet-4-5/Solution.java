import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] workScores = new int[n];
    int[] workIndices = new int[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      System.arraycopy(scores, 0, workScores, 0, n);
      for (int i = 0; i < n; i++) workIndices[i] = i;
      
      // Quickselect to partition around k-th element
      quickSelect(workScores, workIndices, 0, n - 1, k - 1);
      
      // Sort only the top-k elements
      sortTopK(workScores, workIndices, 0, k - 1);
      
      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) workScores[i]) << 32) ^ (workIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickSelect(int[] scores, int[] indices, int left, int right, int k) {
    while (left < right) {
      int pivotIdx = partition(scores, indices, left, right);
      if (pivotIdx == k) return;
      if (pivotIdx > k) right = pivotIdx - 1;
      else left = pivotIdx + 1;
    }
  }
  
  private static int partition(int[] scores, int[] indices, int left, int right) {
    // Median-of-three pivot selection
    int mid = left + (right - left) / 2;
    if (compare(scores[mid], indices[mid], scores[left], indices[left]) > 0) {
      swap(scores, indices, left, mid);
    }
    if (compare(scores[right], indices[right], scores[left], indices[left]) > 0) {
      swap(scores, indices, left, right);
    }
    if (compare(scores[mid], indices[mid], scores[right], indices[right]) > 0) {
      swap(scores, indices, mid, right);
    }
    
    int pivotScore = scores[right];
    int pivotIndex = indices[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      if (compare(scores[j], indices[j], pivotScore, pivotIndex) > 0) {
        i++;
        swap(scores, indices, i, j);
      }
    }
    swap(scores, indices, i + 1, right);
    return i + 1;
  }
  
  private static void sortTopK(int[] scores, int[] indices, int left, int right) {
    int len = right - left + 1;
    if (len <= 1) return;
    
    // Insertion sort for small arrays
    if (len <= 16) {
      for (int i = left + 1; i <= right; i++) {
        int scoreKey = scores[i];
        int indexKey = indices[i];
        int j = i - 1;
        while (j >= left && compare(scores[j], indices[j], scoreKey, indexKey) < 0) {
          scores[j + 1] = scores[j];
          indices[j + 1] = indices[j];
          j--;
        }
        scores[j + 1] = scoreKey;
        indices[j + 1] = indexKey;
      }
      return;
    }
    
    // Quicksort for larger arrays
    int pivotIdx = partition(scores, indices, left, right);
    sortTopK(scores, indices, left, pivotIdx - 1);
    sortTopK(scores, indices, pivotIdx + 1, right);
  }
  
  private static int compare(int score1, int index1, int score2, int index2) {
    if (score1 != score2) return Integer.compare(score1, score2);
    return Integer.compare(index1, index2);
  }
  
  private static void swap(int[] scores, int[] indices, int i, int j) {
    int tempScore = scores[i];
    scores[i] = scores[j];
    scores[j] = tempScore;
    
    int tempIndex = indices[i];
    indices[i] = indices[j];
    indices[j] = tempIndex;
  }
}