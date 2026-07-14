public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0L;
    int n = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short level = levels[i];
        short len = lens[i];

        h ^= (id & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 8) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 16) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 24) & 0xFFL);
        h *= FNV_PRIME;

        h ^= (t & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 8) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 16) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 24) & 0xFFL);
        h *= FNV_PRIME;

        h ^= (level & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((level >>> 8) & 0xFFL);
        h *= FNV_PRIME;

        h ^= (len & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((len >>> 8) & 0xFFL);
        h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}