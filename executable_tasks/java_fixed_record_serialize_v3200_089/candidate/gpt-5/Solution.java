public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    long resultHash = 0L;

    final int n = ids.length;
    final int[] idsArr = ids;
    final int[] tsArr = ts;
    final short[] levelsArr = levels;
    final short[] lensArr = lens;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = idsArr[i];
        h ^= (v & 0xFFL);        h *= FNV_PRIME;
        h ^= ((v >>> 8) & 0xFFL);  h *= FNV_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV_PRIME;

        v = tsArr[i];
        h ^= (v & 0xFFL);        h *= FNV_PRIME;
        h ^= ((v >>> 8) & 0xFFL);  h *= FNV_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV_PRIME;

        int sv = levelsArr[i];
        h ^= (sv & 0xFFL);        h *= FNV_PRIME;
        h ^= ((sv >>> 8) & 0xFFL);  h *= FNV_PRIME;

        sv = lensArr[i];
        h ^= (sv & 0xFFL);        h *= FNV_PRIME;
        h ^= ((sv >>> 8) & 0xFFL);  h *= FNV_PRIME;
      }
      resultHash = h;
    }

    return resultHash;
  }
}
