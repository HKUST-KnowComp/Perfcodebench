public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0;
    final long FNV_OFFSET = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;
    int n = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        h ^= (id & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 8) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 16) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((id >>> 24) & 0xFFL);
        h *= FNV_PRIME;
        int t = ts[i];
        h ^= (t & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 8) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 16) & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((t >>> 24) & 0xFFL);
        h *= FNV_PRIME;
        int level = levels[i];
        h ^= (level & 0xFFL);
        h *= FNV_PRIME;
        h ^= ((level >>> 8) & 0xFFL);
        h *= FNV_PRIME;
        int len = lens[i];
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