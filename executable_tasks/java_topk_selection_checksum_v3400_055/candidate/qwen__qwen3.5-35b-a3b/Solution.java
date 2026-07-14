import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    long h = 0;
    int n = scores.length;
    
    if (k >= n) {
      // Full sort case - use optimized primitive array sort
      int[] sortedScores = new int[n];
      int[] sortedIndices = new int[n];
      for (int i = 0; i < n; i++) {
        sortedScores[i] = scores[i];
        sortedIndices[i] = i;
      }
      
      for (int iter = 0; iter < iters; iter++) {
        Arrays.sort(sortedScores, sortedIndices, 0, n);
        h = 1469598103934665603L;
        for (int i = 0; i < k; i++) {
          h ^= (((long) sortedScores[i]) << 32) ^ (sortedIndices[i] & 0xffffffffL);
          h *= 1099511628211L;
        }
      }
      return h;
    }
    
    // Use min-heap of size k for top-k selection
    int[] topScores = new int[k];
    int[] topIndices = new int[k];
    
    for (int iter = 0; iter < iters; iter++) {
      // Initialize with first k elements
      for (int i = 0; i < k; i++) {
        topScores[i] = scores[i];
        topIndices[i] = i;
      }
      
      // Build min-heap
      for (int i = k / 2 - 1; i >= 0; i--) {
        heapifyDown(topScores, topIndices, i, k);
      }
      
      // Process remaining elements
      for (int i = k; i < n; i++) {
        if (scores[i] > topScores[0] || 
            (scores[i] == topScores[0] && i < topIndices[0])) {
          topScores[0] = scores[i];
          topIndices[0] = i;
          heapifyDown(topScores, topIndices, 0, k);
        }
      }
      
      // Sort top-k for checksum (descending score, ascending index)
      quickSortDesc(topScores, topIndices, 0, k - 1);
      
      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topScores[i]) << 32) ^ (topIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void heapifyDown(int[] scores, int[] indices, int i, int k) {
    int smallest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;
    
    if (left < k) {
      if (scores[left] < scores[smallest] || 
          (scores[left] == scores[smallest] && indices[left] < indices[smallest])) {
        smallest = left;
      }
    }
    
    if (right < k) {
      if (scores[right] < scores[smallest] || 
          (scores[right] == scores[smallest] && indices[right] < indices[smallest])) {
        smallest = right;
      }
    }
    
    if (smallest != i) {
      int tempScore = scores[i];
      scores[i] = scores[smallest];
      scores[smallest] = tempScore;
      
      int tempIndex = indices[i];
      indices[i] = indices[smallest];
      indices[smallest] = tempIndex;
      
      heapifyDown(scores, indices, smallest, k);
    }
  }
  
  private static void quickSortDesc(int[] scores, int[] indices, int left, int right) {
    if (left >= right) return;
    
    int pivotIndex = partitionDesc(scores, indices, left, right);
    quickSortDesc(scores, indices, left, pivotIndex - 1);
    quickSortDesc(scores, indices, pivotIndex + 1, right);
  }
  
  private static int partitionDesc(int[] scores, int[] indices, int left, int right) {
    int pivotScore = scores[right];
    int pivotIndex = indices[right];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      if (scores[j] > pivotScore || 
          (scores[j] == pivotScore && indices[j] < pivotIndex)) {
        i++;
        int tempScore = scores[i];
        scores[i] = scores[j];
        scores[j] = tempScore;
        
        int tempIndex = indices[i];
        indices[i] = indices[j];
        indices[j] = tempIndex;
      }
    }
    
    i++;
    int tempScore = scores[i];
    scores[i] = scores[right];
    scores[right] = tempScore;
    
    int tempIndex = indices[i];
    indices[i] = indices[right];
    indices[right] = tempIndex;
    
    return i;
  }
}