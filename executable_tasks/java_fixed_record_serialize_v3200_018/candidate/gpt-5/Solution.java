public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    if (iters <= 0) return 0L;

    long hash = FNV_OFFSET_BASIS;
    final int n = ids.length; // assuming all arrays have equal length

    for (int i = 0; i < n; i++) {
      int v = ids[i];
      hash ^= (v      ) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>> 8) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>>16) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>>24) & 0xFFL; hash *= FNV_PRIME;

      v = ts[i];
      hash ^= (v      ) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>> 8) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>>16) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (v >>>24) & 0xFFL; hash *= FNV_PRIME;

      int s = levels[i] & 0xFFFF;
      hash ^= (s      ) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (s >>> 8) & 0xFFL; hash *= FNV_PRIME;

      s = lens[i] & 0xFFFF;
      hash ^= (s      ) & 0xFFL; hash *= FNV_PRIME;
      hash ^= (s >>> 8) & 0xFFL; hash *= FNV_PRIME;
    }

    return hash;
  }
}
