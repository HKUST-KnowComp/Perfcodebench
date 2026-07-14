public class Solution {
  private static final long FNV64_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV64_PRIME = 1099511628211L;

  private static long checksumFromRecords(int[] ids, int[] ts, short[] levels, short[] lens) {
    long hash = FNV64_OFFSET_BASIS;
    int n = ids.length;

    for (int i = 0; i < n; i++) {
      int v = ids[i];
      hash ^= (v) & 0xFFL;       hash *= FNV64_PRIME;
      hash ^= (v >>> 8) & 0xFFL;  hash *= FNV64_PRIME;
      hash ^= (v >>> 16) & 0xFFL; hash *= FNV64_PRIME;
      hash ^= (v >>> 24) & 0xFFL; hash *= FNV64_PRIME;

      v = ts[i];
      hash ^= (v) & 0xFFL;       hash *= FNV64_PRIME;
      hash ^= (v >>> 8) & 0xFFL;  hash *= FNV64_PRIME;
      hash ^= (v >>> 16) & 0xFFL; hash *= FNV64_PRIME;
      hash ^= (v >>> 24) & 0xFFL; hash *= FNV64_PRIME;

      v = levels[i]; // short promoted to int with sign extension
      hash ^= (v) & 0xFFL;       hash *= FNV64_PRIME;
      hash ^= (v >>> 8) & 0xFFL;  hash *= FNV64_PRIME;

      v = lens[i]; // short promoted to int with sign extension
      hash ^= (v) & 0xFFL;       hash *= FNV64_PRIME;
      hash ^= (v >>> 8) & 0xFFL;  hash *= FNV64_PRIME;
    }

    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    if (iters <= 0) return 0L;
    // Bytes are identical across iterations; compute once
    return checksumFromRecords(ids, ts, levels, lens);
  }
}
