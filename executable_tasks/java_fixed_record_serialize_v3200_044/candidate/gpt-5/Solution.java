public class Solution {
  private static final long FNV64_OFFSET = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  static long checksumBytes(byte[] data) {
    long hash = FNV64_OFFSET;
    for (int i = 0, n = data.length; i < n; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= FNV64_PRIME;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    long hash = 0L;
    int n = ids.length;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV64_OFFSET;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h ^= (v & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 8) & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV64_PRIME;

        v = ts[i];
        h ^= (v & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 8) & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 16) & 0xFFL); h *= FNV64_PRIME;
        h ^= ((v >>> 24) & 0xFFL); h *= FNV64_PRIME;

        int s = levels[i];
        h ^= (s & 0xFFL); h *= FNV64_PRIME;
        h ^= ((s >>> 8) & 0xFFL); h *= FNV64_PRIME;

        s = lens[i];
        h ^= (s & 0xFFL); h *= FNV64_PRIME;
        h ^= ((s >>> 8) & 0xFFL); h *= FNV64_PRIME;
      }
      hash = h; // last iteration's hash
    }
    return hash;
  }
}
