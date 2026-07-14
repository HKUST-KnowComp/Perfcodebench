public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    // Preserve baseline behavior: if iters == 0, return 0
    long result = 0L;
    final long OFFSET = 1469598103934665603L;
    final long PRIME = 1099511628211L;
    final int n = ids.length;

    for (int iter = 0; iter < iters; iter++) {
      long h = OFFSET;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h ^= (v & 0xFFL); h *= PRIME;
        h ^= ((v >>> 8) & 0xFFL); h *= PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= PRIME;

        int t = ts[i];
        h ^= (t & 0xFFL); h *= PRIME;
        h ^= ((t >>> 8) & 0xFFL); h *= PRIME;
        h ^= ((t >>> 16) & 0xFFL); h *= PRIME;
        h ^= ((t >>> 24) & 0xFFL); h *= PRIME;

        int lv = levels[i]; // short promoted to int with sign extension
        h ^= (lv & 0xFFL); h *= PRIME;
        h ^= ((lv >>> 8) & 0xFFL); h *= PRIME;

        int ln = lens[i];
        h ^= (ln & 0xFFL); h *= PRIME;
        h ^= ((ln >>> 8) & 0xFFL); h *= PRIME;
      }
      result = h; // match baseline: keep only last iteration's checksum
    }

    return result;
  }
}
