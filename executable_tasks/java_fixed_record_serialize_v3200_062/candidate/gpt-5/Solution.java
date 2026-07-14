public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0L;
    final int n = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV64_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h ^= (v) & 0xFFL;      h *= FNV64_PRIME;
        h ^= (v >>> 8) & 0xFFL;  h *= FNV64_PRIME;
        h ^= (v >>> 16) & 0xFFL; h *= FNV64_PRIME;
        h ^= (v >>> 24) & 0xFFL; h *= FNV64_PRIME;

        int t = ts[i];
        h ^= (t) & 0xFFL;      h *= FNV64_PRIME;
        h ^= (t >>> 8) & 0xFFL;  h *= FNV64_PRIME;
        h ^= (t >>> 16) & 0xFFL; h *= FNV64_PRIME;
        h ^= (t >>> 24) & 0xFFL; h *= FNV64_PRIME;

        int lv = levels[i];
        h ^= (lv) & 0xFFL;      h *= FNV64_PRIME;
        h ^= (lv >>> 8) & 0xFFL;  h *= FNV64_PRIME;

        int ln = lens[i];
        h ^= (ln) & 0xFFL;      h *= FNV64_PRIME;
        h ^= (ln >>> 8) & 0xFFL;  h *= FNV64_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}
