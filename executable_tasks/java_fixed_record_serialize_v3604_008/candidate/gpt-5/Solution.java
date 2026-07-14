public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    final int n = ids.length;
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h = (h ^ (v & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 8) & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 16) & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 24) & 0xFFL)) * FNV_PRIME;

        v = ts[i];
        h = (h ^ (v & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 8) & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 16) & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((v >>> 24) & 0xFFL)) * FNV_PRIME;

        int s = levels[i]; // promoted to int
        h = (h ^ (s & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((s >>> 8) & 0xFFL)) * FNV_PRIME;

        s = lens[i];
        h = (h ^ (s & 0xFFL)) * FNV_PRIME;
        h = (h ^ ((s >>> 8) & 0xFFL)) * FNV_PRIME;
      }
      hash = h;
    }

    return hash;
  }
}
