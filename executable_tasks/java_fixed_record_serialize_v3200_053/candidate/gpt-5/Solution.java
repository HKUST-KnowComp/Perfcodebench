public class Solution {
  private static final long FNV_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long outHash = 0L;
    final int n = ids.length;
    final long prime = FNV_PRIME;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_BASIS;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        h ^= (id & 0xFFL);         h *= prime;
        h ^= ((id >>> 8) & 0xFFL); h *= prime;
        h ^= ((id >>> 16) & 0xFFL);h *= prime;
        h ^= ((id >>> 24) & 0xFFL);h *= prime;

        int t = ts[i];
        h ^= (t & 0xFFL);          h *= prime;
        h ^= ((t >>> 8) & 0xFFL);  h *= prime;
        h ^= ((t >>> 16) & 0xFFL); h *= prime;
        h ^= ((t >>> 24) & 0xFFL); h *= prime;

        int lv = levels[i] & 0xFFFF;
        h ^= (lv & 0xFFL);         h *= prime;
        h ^= ((lv >>> 8) & 0xFFL); h *= prime;

        int ln = lens[i] & 0xFFFF;
        h ^= (ln & 0xFFL);         h *= prime;
        h ^= ((ln >>> 8) & 0xFFL); h *= prime;
      }
      outHash = h; // match original behavior: return hash of last iteration
    }

    return outHash;
  }
}
