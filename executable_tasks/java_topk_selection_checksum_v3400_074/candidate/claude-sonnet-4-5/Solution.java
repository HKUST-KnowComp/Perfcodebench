import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] sortedScores = new int[n];
    int[] sortedIndices = new int[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      System.arraycopy(scores, 0, sortedScores, 0, n);
      for (int i = 0; i < n; i++) sortedIndices[i] = i;
      
      // Quickselect to partition around k-th element
      int left = 0, right = n - 1;
      int targetIdx = k - 1;
      
      while (left < right) {
        int pivotIdx = partition(sortedScores, sortedIndices, left, right);
        if (pivotIdx == targetIdx) break;
        else if (pivotIdx < targetIdx) left = pivotIdx + 1;
        else right = pivotIdx - 1;
      }
      
      // Sort only the top-k elements
      insertionSort(sortedScores, sortedIndices, 0, k);
      
      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) sortedScores[i]) << 32) ^ (sortedIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
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
  
  private static void insertionSort(int[] scores, int[] indices, int start, int end) {
    for (int i = start + 1; i < end; i++) {
      int scoreKey = scores[i];
      int indexKey = indices[i];
      int j = i - 1;
      while (j >= start && compare(scores[j], indices[j], scoreKey, indexKey) < 0) {
        scores[j + 1] = scores[j];
        indices[j + 1] = indices[j];
        j--;
      }
      scores[j + 1] = scoreKey;
      indices[j + 1] = indexKey;
    }
  }
  
  private static int compare(int score1, int idx1, int score2, int idx2) {
    if (score1 != score2) return Integer.compare(score1, score2);
    return Integer.compare(idx2, idx1); // Reverse for index
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