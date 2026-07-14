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
      
      quickSelect(workScores, workIndices, 0, n - 1, k);
      
      sortTopK(workScores, workIndices, 0, k - 1);
      
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
      if (pivotIdx == k - 1) return;
      if (pivotIdx > k - 1) right = pivotIdx - 1;
      else left = pivotIdx + 1;
    }
  }
  
  private static int partition(int[] scores, int[] indices, int left, int right) {
    int mid = left + (right - left) / 2;
    int pivotScore = scores[mid];
    int pivotIndex = indices[mid];
    
    swap(scores, indices, mid, right);
    int storeIdx = left;
    
    for (int i = left; i < right; i++) {
      if (scores[i] > pivotScore || (scores[i] == pivotScore && indices[i] < pivotIndex)) {
        swap(scores, indices, i, storeIdx);
        storeIdx++;
      }
    }
    
    swap(scores, indices, storeIdx, right);
    return storeIdx;
  }
  
  private static void sortTopK(int[] scores, int[] indices, int left, int right) {
    for (int i = left + 1; i <= right; i++) {
      int scoreKey = scores[i];
      int indexKey = indices[i];
      int j = i - 1;
      
      while (j >= left && (scores[j] < scoreKey || (scores[j] == scoreKey && indices[j] > indexKey))) {
        scores[j + 1] = scores[j];
        indices[j + 1] = indices[j];
        j--;
      }
      scores[j + 1] = scoreKey;
      indices[j + 1] = indexKey;
    }
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