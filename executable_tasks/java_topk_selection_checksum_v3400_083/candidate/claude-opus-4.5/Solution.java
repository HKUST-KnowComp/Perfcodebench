import java.util.*;

public class Solution {

  public static long run(int[] scores, int k, int iters) {
    final int n = scores.length;
    if (k <= 0 || n == 0) {
      long h = 1469598103934665603L;
      for (int iter = 0; iter < iters; iter++) {
        h = 1469598103934665603L;
      }
      return h;
    }
    if (k > n) k = n;

    // Heap arrays: store (score, index) as two parallel arrays
    // Min-heap by (score ASC, index DESC) so smallest-valued element is at root
    int[] heapScores = new int[k];
    int[] heapIndices = new int[k];
    
    // Result arrays for final sorted top-k
    int[] resultScores = new int[k];
    int[] resultIndices = new int[k];

    long h = 0;
    for (int iter = 0; iter < iters; iter++) {
      int heapSize = 0;

      for (int i = 0; i < n; i++) {
        int score = scores[i];
        if (heapSize < k) {
          // Add to heap
          int pos = heapSize++;
          heapScores[pos] = score;
          heapIndices[pos] = i;
          // Sift up
          while (pos > 0) {
            int parent = (pos - 1) >> 1;
            // Min-heap: parent should be <= child
            // Compare: (score ASC, index DESC)
            if (heapScores[parent] < score || 
                (heapScores[parent] == score && heapIndices[parent] > i)) {
              break;
            }
            heapScores[pos] = heapScores[parent];
            heapIndices[pos] = heapIndices[parent];
            heapScores[parent] = score;
            heapIndices[parent] = i;
            pos = parent;
          }
        } else {
          // Check if current element should replace min
          int minScore = heapScores[0];
          int minIndex = heapIndices[0];
          // Current is better if: score > minScore OR (score == minScore AND i < minIndex)
          if (score > minScore || (score == minScore && i < minIndex)) {
            // Replace root and sift down
            heapScores[0] = score;
            heapIndices[0] = i;
            int pos = 0;
            while (true) {
              int left = (pos << 1) + 1;
              int right = left + 1;
              int smallest = pos;
              int smallestScore = heapScores[pos];
              int smallestIndex = heapIndices[pos];
              
              if (left < k) {
                int ls = heapScores[left];
                int li = heapIndices[left];
                // left < smallest if: ls < smallestScore OR (ls == smallestScore AND li > smallestIndex)
                if (ls < smallestScore || (ls == smallestScore && li > smallestIndex)) {
                  smallest = left;
                  smallestScore = ls;
                  smallestIndex = li;
                }
              }
              if (right < k) {
                int rs = heapScores[right];
                int ri = heapIndices[right];
                if (rs < smallestScore || (rs == smallestScore && ri > smallestIndex)) {
                  smallest = right;
                  smallestScore = rs;
                  smallestIndex = ri;
                }
              }
              if (smallest == pos) break;
              heapScores[pos] = smallestScore;
              heapIndices[pos] = smallestIndex;
              heapScores[smallest] = score;
              heapIndices[smallest] = i;
              pos = smallest;
            }
          }
        }
      }

      // Extract all k elements in sorted order (descending score, ascending index)
      int extractSize = heapSize;
      for (int i = extractSize - 1; i >= 0; i--) {
        resultScores[i] = heapScores[0];
        resultIndices[i] = heapIndices[0];
        // Move last to root and sift down
        heapSize--;
        if (heapSize > 0) {
          int score = heapScores[heapSize];
          int idx = heapIndices[heapSize];
          heapScores[0] = score;
          heapIndices[0] = idx;
          int pos = 0;
          while (true) {
            int left = (pos << 1) + 1;
            int right = left + 1;
            int smallest = pos;
            int smallestScore = heapScores[pos];
            int smallestIndex = heapIndices[pos];
            
            if (left < heapSize) {
              int ls = heapScores[left];
              int li = heapIndices[left];
              if (ls < smallestScore || (ls == smallestScore && li > smallestIndex)) {
                smallest = left;
                smallestScore = ls;
                smallestIndex = li;
              }
            }
            if (right < heapSize) {
              int rs = heapScores[right];
              int ri = heapIndices[right];
              if (rs < smallestScore || (rs == smallestScore && ri > smallestIndex)) {
                smallest = right;
                smallestScore = rs;
                smallestIndex = ri;
              }
            }
            if (smallest == pos) break;
            heapScores[pos] = smallestScore;
            heapIndices[pos] = smallestIndex;
            heapScores[smallest] = score;
            heapIndices[smallest] = idx;
            pos = smallest;
          }
        }
      }

      // Compute checksum
      h = 1469598103934665603L;
      for (int i = 0; i < extractSize; i++) {
        h ^= (((long) resultScores[i]) << 32) ^ (resultIndices[i] & 0xffffffffL);
        h *= 1099511628211L;
      }
    }
    return h;
  }
}
