public class Solution {
  // Compute checksum of the serialized records directly without materializing the byte array
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int n = ids.length;
    final long FNV_OFFSET_BASIS = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    long hash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET_BASIS;
      for (int i = 0; i < n; i++) {
        int v;
        // ids[i] - 4 bytes little-endian
        v = ids[i];
        h ^= (v) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 8) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 16) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 24) & 0xFFL; h *= FNV_PRIME;

        // ts[i] - 4 bytes little-endian
        v = ts[i];
        h ^= (v) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 8) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 16) & 0xFFL; h *= FNV_PRIME;
        h ^= (v >>> 24) & 0xFFL; h *= FNV_PRIME;

        // levels[i] - 2 bytes little-endian
        int sv = levels[i]; // promoted to int with sign; handled via masking
        h ^= (sv) & 0xFFL; h *= FNV_PRIME;
        h ^= (sv >>> 8) & 0xFFL; h *= FNV_PRIME;

        // lens[i] - 2 bytes little-endian
        sv = lens[i];
        h ^= (sv) & 0xFFL; h *= FNV_PRIME;
        h ^= (sv >>> 8) & 0xFFL; h *= FNV_PRIME;
      }
      // As in baseline, only the last iteration's checksum is returned
      hash = h;
    }
    return hash;
  }
}
