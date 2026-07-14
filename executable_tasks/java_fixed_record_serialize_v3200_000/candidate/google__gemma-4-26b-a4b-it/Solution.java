import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 146959810394665603L; // Note: The original code had a typo in the constant 1469598103934665603L vs standard FNV, but I must preserve the exact logic provided in the baseline.
    // Re-evaluating baseline: hash = 1469598103934665603L;
    // I will use the exact constant from the baseline to ensure correctness.
    hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int recordSize = 4 + 4 + 2 + 2; // int, int, short, short
    int totalSize = ids.length * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        buffer[offset++] = (byte) id;
        buffer[offset++] = (byte) (id >>> 8);
        buffer[offset++] = (byte) (id >>> 16);
        buffer[offset++] = (byte) (id >>> 24);

        int t = ts[i];
        buffer[offset++] = (byte) t;
        buffer[offset++] = (byte) (t >>> 8);
        buffer[offset++] = (byte) (t >>> 16);
        buffer[offset++] = (byte) (t >>> 24);

        short lvl = levels[i];
        buffer[offset++] = (byte) lvl;
        buffer[offset++] = (byte) (lvl >>> 8);

        short ln = lens[i];
        buffer[offset++] = (byte) ln;
        buffer[offset++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}