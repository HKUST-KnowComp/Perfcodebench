public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    if (iters <= 0) return 0L;

    long hash = FNV64_OFFSET;
    int n = ids.length;

    for (int i = 0; i < n; i++) {
      int id = ids[i];
      long b;
      b = (id) & 0xFFL;       hash ^= b; hash *= FNV64_PRIME;
      b = (id >>> 8) & 0xFFL;  hash ^= b; hash *= FNV64_PRIME;
      b = (id >>> 16) & 0xFFL; hash ^= b; hash *= FNV64_PRIME;
      b = (id >>> 24) & 0xFFL; hash ^= b; hash *= FNV64_PRIME;

      int t = ts[i];
      b = (t) & 0xFFL;         hash ^= b; hash *= FNV64_PRIME;
      b = (t >>> 8) & 0xFFL;   hash ^= b; hash *= FNV64_PRIME;
      b = (t >>> 16) & 0xFFL;  hash ^= b; hash *= FNV64_PRIME;
      b = (t >>> 24) & 0xFFL;  hash ^= b; hash *= FNV64_PRIME;

      int lvl = levels[i] & 0xFFFF;
      b = (lvl) & 0xFFL;       hash ^= b; hash *= FNV64_PRIME;
      b = (lvl >>> 8) & 0xFFL; hash ^= b; hash *= FNV64_PRIME;

      int ln = lens[i] & 0xFFFF;
      b = (ln) & 0xFFL;        hash ^= b; hash *= FNV64_PRIME;
      b = (ln >>> 8) & 0xFFL;  hash ^= b; hash *= FNV64_PRIME;
    }

    return hash;
  }
}
