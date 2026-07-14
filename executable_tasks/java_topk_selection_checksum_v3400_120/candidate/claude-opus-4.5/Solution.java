import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    if (k <= 0) {
      long h = 1469598103934665603L;
      return h;
    }
    if (k > n) k = n;
    
    int[] workScores = new int[n];
    int[] workIndices = new int[n];
    
    int[] topScores = new int[k];
    int[] topIndices = new int[k];
    
    long h = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < n; i++) {
        workScores[i] = scores[i];
        workIndices[i] = i;
      }
      
      quickSelectTopK(workScores, workIndices, 0, n - 1, k);
      
      for (int i = 0; i < k; i++) {
        topScores[i] = workScores[i];
        topIndices[i] = workIndices[i];
      }
      sortTopK(topScores, topIndices, k);
      
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topScores[i]) << 32) ^ (topIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  private static void quickSelectTopK(int[] scores, int[] indices, int left, int right, int k) {
    while (left < right) {
      int pivotIdx = medianOfThree(scores, indices, left, right);
      pivotIdx = partition(scores, indices, left, right, pivotIdx);
      
      if (pivotIdx == k - 1) {
        return;
      } else if (pivotIdx > k - 1) {
        right = pivotIdx - 1;
      } else {
        left = pivotIdx + 1;
      }
    }
  }
  
  private static int medianOfThree(int[] scores, int[] indices, int left, int right) {
    int mid = left + (right - left) / 2;
    
    if (compareDesc(scores, indices, left, mid) > 0) {
      swap(scores, indices, left, mid);
    }
    if (compareDesc(scores, indices, left, right) > 0) {
      swap(scores, indices, left, right);
    }
    if (compareDesc(scores, indices, mid, right) > 0) {
      swap(scores, indices, mid, right);
    }
    return mid;
  }
  
  private static int compareDesc(int[] scores, int[] indices, int i, int j) {
    if (scores[i] != scores[j]) {
      return Integer.compare(scores[j], scores[i]);
    }
    return Integer.compare(indices[i], indices[j]);
  }
  
  private static int partition(int[] scores, int[] indices, int left, int right, int pivotIdx) {
    int pivotScore = scores[pivotIdx];
    int pivotIndex = indices[pivotIdx];
    
    swap(scores, indices, pivotIdx, right);
    
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
  
  private static void swap(int[] scores, int[] indices, int i, int j) {
    int ts = scores[i];
    scores[i] = scores[j];
    scores[j] = ts;
    int ti = indices[i];
    indices[i] = indices[j];
    indices[j] = ti;
  }
  
  private static void sortTopK(int[] scores, int[] indices, int k) {
    for (int i = 1; i < k; i++) {
      int score = scores[i];
      int index = indices[i];
      int j = i - 1;
      while (j >= 0 && (scores[j] < score || (scores[j] == score && indices[j] > index))) {
        scores[j + 1] = scores[j];
        indices[j + 1] = indices[j];
        j--;
      }
      scores[j + 1] = score;
      indices[j + 1] = index;
    }
  }
}