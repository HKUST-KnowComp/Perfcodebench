public class Solution {
  public static long run(int[] keys, int[] vals, int buckets, int iters) {
    long h = 0;
    // Preallocate all arrays once to avoid repeated allocation/GC
    long[] hist = new long[buckets];
    int[] modifiedIndices = new int[keys.length];
    
    for (int iter = 0; iter < iters; iter++) {
      int modCount = 0;
      // Fill histogram and track which indices were modified
      for (int i = 0; i < keys.length; i++) {
        int key = keys[i];
        hist[key] += vals[i];
        modifiedIndices[modCount++] = key;
      }
      // Compute checksum exactly as original to preserve correctness
      h = 1469598103934665603L;
      for (long v : hist) {
        h ^= (v + 1);
        h *= 1099511628211L;
      }
      // Reset only modified indices to zero for next iteration
      for (int m = 0; m < modCount; m++) {
        hist[modifiedIndices[m]] = 0;
      }
    }
    return h;
  }
}