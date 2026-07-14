import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    int n = scores.length;
    if (k <= 0) {
      long h = 1469598103934665603L;
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }
    if (k > n) k = n;
    
    // Reusable arrays for indices and for final top-k
    int[] idx = new int[n];
    int[] topScores = new int[k];
    int[] topIndices = new int[k];
    
    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      // Initialize index array
      for (int i = 0; i < n; i++) idx[i] = i;
      
      // Quickselect to partition so that top k are in idx[0..k-1]
      quickselect(scores, idx, 0, n - 1, k);
      
      // Extract top k
      for (int i = 0; i < k; i++) {
        topIndices[i] = idx[i];
        topScores[i] = scores[idx[i]];
      }
      
      // Sort only the top k elements
      sortTopK(topScores, topIndices, k);
      
      // Compute hash
      h = 1469598103934665603L;
      for (int i = 0; i < k; i++) {
        h ^= (((long) topScores[i]) << 32) ^ (topIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
  
  // Quickselect: partition so that the k largest (by score desc, then index asc) are in idx[0..k-1]
  private static void quickselect(int[] scores, int[] idx, int left, int right, int k) {
    while (left < right) {
      int pivotIndex = partition(scores, idx, left, right);
      int count = pivotIndex - left + 1;
      if (count == k) {
        return;
      } else if (count > k) {
        right = pivotIndex - 1;
      } else {
        left = pivotIndex + 1;
        k -= count;
      }
    }
  }
  
  private static int partition(int[] scores, int[] idx, int left, int right) {
    // Median-of-three pivot selection
    int mid = left + (right - left) / 2;
    if (compare(scores, idx, left, mid) < 0) swap(idx, left, mid);
    if (compare(scores, idx, left, right) < 0) swap(idx, left, right);
    if (compare(scores, idx, mid, right) < 0) swap(idx, mid, right);
    swap(idx, mid, right); // pivot at right
    
    int pivotIdx = idx[right];
    int pivotScore = scores[pivotIdx];
    int i = left - 1;
    
    for (int j = left; j < right; j++) {
      int jIdx = idx[j];
      int jScore = scores[jIdx];
      // We want descending by score, ascending by index
      // So "greater" means: higher score, or same score with lower index
      if (jScore > pivotScore || (jScore == pivotScore && jIdx < pivotIdx)) {
        i++;
        swap(idx, i, j);
      }
    }
    swap(idx, i + 1, right);
    return i + 1;
  }
  
  // Compare for descending score, ascending index
  // Returns positive if idx[a] should come before idx[b]
  private static int compare(int[] scores, int[] idx, int a, int b) {
    int sa = scores[idx[a]];
    int sb = scores[idx[b]];
    if (sa != sb) return sa - sb; // higher score = should come first = positive
    return idx[b] - idx[a]; // lower index = should come first = positive when b > a
  }
  
  private static void swap(int[] arr, int i, int j) {
    int tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
  }
  
  // Insertion sort for small k, otherwise use a simple sort
  private static void sortTopK(int[] topScores, int[] topIndices, int k) {
    // Insertion sort - good for small k
    for (int i = 1; i < k; i++) {
      int score = topScores[i];
      int index = topIndices[i];
      int j = i - 1;
      // Descending by score, ascending by index
      while (j >= 0 && (topScores[j] < score || (topScores[j] == score && topIndices[j] > index))) {
        topScores[j + 1] = topScores[j];
        topIndices[j + 1] = topIndices[j];
        j--;
      }
      topScores[j + 1] = score;
      topIndices[j + 1] = index;
    }
  }
}