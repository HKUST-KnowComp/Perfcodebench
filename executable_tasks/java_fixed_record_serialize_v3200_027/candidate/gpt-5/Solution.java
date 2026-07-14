public class Solution {
  // Kept for compatibility/testing if needed, though not used by run.
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final long P = 1099511628211L; // FNV-1a 64-bit prime
    long hash = 0L;
    final int n = ids.length;

    for (int iter = 0; iter < iters; iter++) {
      long h = 1469598103934665603L; // FNV-1a 64-bit offset basis

      for (int i = 0; i < n; i++) {
        int v;
        // ids[i] - 4 bytes little endian
        v = ids[i];
        h ^= (v & 0xFFL);
        h *= P;
        h ^= ((v >>> 8) & 0xFFL);
        h *= P;
        h ^= ((v >>> 16) & 0xFFL);
        h *= P;
        h ^= ((v >>> 24) & 0xFFL);
        h *= P;

        // ts[i] - 4 bytes little endian
        v = ts[i];
        h ^= (v & 0xFFL);
        h *= P;
        h ^= ((v >>> 8) & 0xFFL);
        h *= P;
        h ^= ((v >>> 16) & 0xFFL);
        h *= P;
        h ^= ((v >>> 24) & 0xFFL);
        h *= P;

        // levels[i] - 2 bytes little endian
        int sv = levels[i] & 0xFFFF;
        h ^= (sv & 0xFFL);
        h *= P;
        h ^= ((sv >>> 8) & 0xFFL);
        h *= P;

        // lens[i] - 2 bytes little endian
        sv = lens[i] & 0xFFFF;
        h ^= (sv & 0xFFL);
        h *= P;
        h ^= ((sv >>> 8) & 0xFFL);
        h *= P;
      }

      // As in the baseline, return the checksum of the last iteration
      hash = h;
    }

    return hash;
  }
}
