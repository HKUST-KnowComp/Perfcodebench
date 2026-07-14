import java.util.*;

public class Solution {
  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    int[] tempScores = new int[n];
    int[] tempIndices = new int[n];
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      System.arraycopy(scores, 0, tempScores, 0, n);
      for (int i = 0; i < n; i++) tempIndices[i] = i;
      
      partialSort(tempScores, tempIndices, 0, n - 1, k);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) tempScores[i]) << 32) ^ (tempIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void partialSort(int[] scores, int[] indices, int left, int right, int k) {
    if (left >= right || k <= 0) return;
    
    int pivotIdx = partition(scores, indices, left, right);
    int numLeft = pivotIdx - left + 1;
    
    if (numLeft == k) {
      return;
    } else if (numLeft > k) {
      partialSort(scores, indices, left, pivotIdx - 1, k);
    } else {
      partialSort(scores, indices, pivotIdx + 1, right, k - numLeft);
    }
  }
  
  private static int partition(int[] scores, int[] indices, int left, int right) {
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
  
  private static int compare(int score1, int idx1, int score2, int idx2) {
    if (score1 != score2) return Integer.compare(score1, score2);
    return Integer.compare(idx2, idx1);
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