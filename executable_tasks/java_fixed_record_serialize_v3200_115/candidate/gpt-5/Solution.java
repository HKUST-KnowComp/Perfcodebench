public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int n = ids.length;
    final long FNV64_OFFSET = 1469598103934665603L;
    final long FNV64_PRIME = 1099511628211L;

    long lastHash = 0L;

    // Local references for slight performance improvement
    final int[] idsArr = ids;
    final int[] tsArr = ts;
    final short[] lvlArr = levels;
    final short[] lenArr = lens;

    for (int iter = 0; iter < iters; iter++) {
      long hash = FNV64_OFFSET;

      for (int i = 0; i < n; i++) {
        int v = idsArr[i];
        hash ^= (v) & 0xFFL;        hash *= FNV64_PRIME;
        hash ^= (v >>> 8) & 0xFFL;  hash *= FNV64_PRIME;
        hash ^= (v >>> 16) & 0xFFL; hash *= FNV64_PRIME;
        hash ^= (v >>> 24) & 0xFFL; hash *= FNV64_PRIME;

        int t = tsArr[i];
        hash ^= (t) & 0xFFL;        hash *= FNV64_PRIME;
        hash ^= (t >>> 8) & 0xFFL;  hash *= FNV64_PRIME;
        hash ^= (t >>> 16) & 0xFFL; hash *= FNV64_PRIME;
        hash ^= (t >>> 24) & 0xFFL; hash *= FNV64_PRIME;

        int lv = lvlArr[i] & 0xFFFF;
        hash ^= lv & 0xFFL;         hash *= FNV64_PRIME;
        hash ^= (lv >>> 8) & 0xFFL; hash *= FNV64_PRIME;

        int ln = lenArr[i] & 0xFFFF;
        hash ^= ln & 0xFFL;         hash *= FNV64_PRIME;
        hash ^= (ln >>> 8) & 0xFFL; hash *= FNV64_PRIME;
      }

      lastHash = hash;
    }

    return lastHash;
  }
}