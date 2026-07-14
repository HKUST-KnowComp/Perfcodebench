public class Solution {
  public static long run(int[] ids, int[] dictVals, int iters) {
    long h = 0;
    final long FNV_PRIME = 1099511628211L;
    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final int len = ids.length;

    for (int iter = 0; iter < iters; iter++) {
      h = FNV_OFFSET_BASIS;
      int i = 0;
      
      // Unroll the loop to reduce loop overhead and improve instruction-level parallelism
      // for the memory access patterns.
      for (; i <= len - 8; i += 8) {
        h = (h ^ ((dictVals[ids[i]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 1]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 2]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 3]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 4]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 5]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 6]] & 0xffffffffL) + 1L)) * FNV_PRIME;
        h = (h ^ ((dictVals[ids[i + 7]] & 0xffffffffL) + 1L)) * FNV_PRIME;
      }

      // Handle remaining elements
      for (; i < len; i++) {
        int v = dictVals[ids[i]];
        h ^= (v & 0xffffffffL) + 1L;
        h *= FNV_PRIME;
      }
    }
    return h;
  }
}