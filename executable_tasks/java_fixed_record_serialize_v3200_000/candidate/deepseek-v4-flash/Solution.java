import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int n = ids.length;
    long resultHash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long hash = FNV_OFFSET;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        hash ^= (byte) id & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (id >>> 8) & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (id >>> 16) & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (id >>> 24) & 0xFFL;
        hash *= FNV_PRIME;

        int tsVal = ts[i];
        hash ^= (byte) tsVal & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (tsVal >>> 8) & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (tsVal >>> 16) & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (tsVal >>> 24) & 0xFFL;
        hash *= FNV_PRIME;

        short level = levels[i];
        hash ^= (byte) level & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (level >>> 8) & 0xFFL;
        hash *= FNV_PRIME;

        short len = lens[i];
        hash ^= (byte) len & 0xFFL;
        hash *= FNV_PRIME;
        hash ^= (byte) (len >>> 8) & 0xFFL;
        hash *= FNV_PRIME;
      }
      resultHash = hash;
    }
    return resultHash;
  }
}