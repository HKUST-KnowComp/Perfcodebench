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
      
      // QuickSelect to partition top k elements
      quickSelect(sortedScores, sortedIndices, 0, n - 1, k);
      
      // Sort top k by score desc, then index asc
      sortTopK(sortedScores, sortedIndices, 0, k - 1);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) sortedScores[i]) << 32) ^ (sortedIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickSelect(int[] scores, int[] indices, int left, int right, int k) {
    while (left < right) {
      int pivotIdx = partition(scores, indices, left, right);
      if (pivotIdx == k - 1) break;
      else if (pivotIdx < k - 1) left = pivotIdx + 1;
      else right = pivotIdx - 1;
    }
  }
  
  private static int partition(int[] scores, int[] indices, int left, int right) {
    int pivotIdx = left + (right - left) / 2;
    int pivotScore = scores[pivotIdx];
    int pivotIndex = indices[pivotIdx];
    
    swap(scores, indices, pivotIdx, right);
    int storeIdx = left;
    
    for (int i = left; i < right; i++) {
      if (scores[i] > pivotScore || (scores[i] == pivotScore && indices[i] < pivotIndex)) {
        swap(scores, indices, i, storeIdx++);
      }
    }
    swap(scores, indices, storeIdx, right);
    return storeIdx;
  }
  
  private static void sortTopK(int[] scores, int[] indices, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;
    sortTopK(scores, indices, left, mid);
    sortTopK(scores, indices, mid + 1, right);
    merge(scores, indices, left, mid, right);
  }
  
  private static void merge(int[] scores, int[] indices, int left, int mid, int right) {
    int[] tempScores = new int[right - left + 1];
    int[] tempIndices = new int[right - left + 1];
    int i = left, j = mid + 1, k = 0;
    
    while (i <= mid && j <= right) {
      if (scores[i] > scores[j] || (scores[i] == scores[j] && indices[i] < indices[j])) {
        tempScores[k] = scores[i];
        tempIndices[k] = indices[i++];
      } else {
        tempScores[k] = scores[j];
        tempIndices[k] = indices[j++];
      }
      k++;
    }
    while (i <= mid) {
      tempScores[k] = scores[i];
      tempIndices[k] = indices[i++];
      k++;
    }
    while (j <= right) {
      tempScores[k] = scores[j];
      tempIndices[k] = indices[j++];
      k++;
    }
    
    for (k = 0; k < tempScores.length; k++) {
      scores[left + k] = tempScores[k];
      indices[left + k] = tempIndices[k];
    }
  }
  
  private static void swap(int[] scores, int[] indices, int i, int j) {
    int tmpScore = scores[i];
    scores[i] = scores[j];
    scores[j] = tmpScore;
    int tmpIdx = indices[i];
    indices[i] = indices[j];
    indices[j] = tmpIdx;
  }
}