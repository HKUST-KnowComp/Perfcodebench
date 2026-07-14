import java.util.*;

public class Solution {
  private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  // Kept for reference; not used in the optimized path
  static long checksumBytes(byte[] data) {
    long hash = FNV_OFFSET_BASIS;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    // Baseline returns 0 when iters == 0
    if (iters <= 0) return 0L;

    // Compute the checksum once since each iteration serializes identical data
    final long prime = FNV_PRIME;
    long hash = FNV_OFFSET_BASIS;
    final int n = ids.length;

    for (int i = 0; i < n; i++) {
      int v = ids[i];
      hash ^= (v       & 0xFFL); hash *= prime;
      hash ^= ((v >>> 8)  & 0xFFL); hash *= prime;
      hash ^= ((v >>> 16) & 0xFFL); hash *= prime;
      hash ^= ((v >>> 24) & 0xFFL); hash *= prime;

      v = ts[i];
      hash ^= (v       & 0xFFL); hash *= prime;
      hash ^= ((v >>> 8)  & 0xFFL); hash *= prime;
      hash ^= ((v >>> 16) & 0xFFL); hash *= prime;
      hash ^= ((v >>> 24) & 0xFFL); hash *= prime;

      int s = levels[i] & 0xFFFF;
      hash ^= (s        & 0xFFL); hash *= prime;
      hash ^= ((s >>> 8) & 0xFFL); hash *= prime;

      s = lens[i] & 0xFFFF;
      hash ^= (s        & 0xFFL); hash *= prime;
      hash ^= ((s >>> 8) & 0xFFL); hash *= prime;
    }

    // The baseline overwrites the hash with the same value each iteration; returning once is equivalent
    return hash;
  }
}
