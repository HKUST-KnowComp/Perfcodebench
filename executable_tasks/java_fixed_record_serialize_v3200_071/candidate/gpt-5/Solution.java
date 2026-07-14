public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    if (iters == 0) return 0L;
    final int n = ids.length;
    long hash = FNV_OFFSET_BASIS;

    for (int i = 0; i < n; i++) {
      int id = ids[i];
      hash ^= (id & 0xFFL);         hash *= FNV_PRIME;
      hash ^= ((id >>> 8) & 0xFFL); hash *= FNV_PRIME;
      hash ^= ((id >>> 16) & 0xFFL);hash *= FNV_PRIME;
      hash ^= ((id >>> 24) & 0xFFL);hash *= FNV_PRIME;

      int t = ts[i];
      hash ^= (t & 0xFFL);          hash *= FNV_PRIME;
      hash ^= ((t >>> 8) & 0xFFL);  hash *= FNV_PRIME;
      hash ^= ((t >>> 16) & 0xFFL); hash *= FNV_PRIME;
      hash ^= ((t >>> 24) & 0xFFL); hash *= FNV_PRIME;

      int lv = levels[i] & 0xFFFF;
      hash ^= (lv & 0xFFL);         hash *= FNV_PRIME;
      hash ^= ((lv >>> 8) & 0xFFL); hash *= FNV_PRIME;

      int ln = lens[i] & 0xFFFF;
      hash ^= (ln & 0xFFL);         hash *= FNV_PRIME;
      hash ^= ((ln >>> 8) & 0xFFL); hash *= FNV_PRIME;
    }

    return hash;
  }
}
