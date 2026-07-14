public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0L;
    final int n = ids.length; // Assumes matching lengths as in baseline
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h ^= (v & 0xFFL);        h *= FNV_PRIME;
        h ^= ((v >>> 8) & 0xFFL);  h *= FNV_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV_PRIME;

        v = ts[i];
        h ^= (v & 0xFFL);        h *= FNV_PRIME;
        h ^= ((v >>> 8) & 0xFFL);  h *= FNV_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV_PRIME;

        int sv = levels[i];
        h ^= (sv & 0xFFL);        h *= FNV_PRIME;
        h ^= ((sv >>> 8) & 0xFFL);  h *= FNV_PRIME;

        sv = lens[i];
        h ^= (sv & 0xFFL);        h *= FNV_PRIME;
        h ^= ((sv >>> 8) & 0xFFL);  h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}
