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
    long hash = 0L;
    final int n = ids.length;
    final long prime = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        h ^= (v & 0xFFL);        h *= prime;
        h ^= ((v >>> 8) & 0xFFL);  h *= prime;
        h ^= ((v >>> 16) & 0xFFL); h *= prime;
        h ^= ((v >>> 24) & 0xFFL); h *= prime;

        v = ts[i];
        h ^= (v & 0xFFL);        h *= prime;
        h ^= ((v >>> 8) & 0xFFL);  h *= prime;
        h ^= ((v >>> 16) & 0xFFL); h *= prime;
        h ^= ((v >>> 24) & 0xFFL); h *= prime;

        int s = levels[i] & 0xFFFF;
        h ^= (s & 0xFFL);        h *= prime;
        h ^= ((s >>> 8) & 0xFFL);  h *= prime;

        s = lens[i] & 0xFFFF;
        h ^= (s & 0xFFL);        h *= prime;
        h ^= ((s >>> 8) & 0xFFL);  h *= prime;
      }
      hash = h; // checksum of the serialized buffer for this iteration
    }
    return hash;
  }
}