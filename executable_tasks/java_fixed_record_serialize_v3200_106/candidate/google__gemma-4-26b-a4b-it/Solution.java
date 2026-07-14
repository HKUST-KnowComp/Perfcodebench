import java.util.*;

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
    int recordSize = 4 + 4 + 2 + 2; // id (4), ts (4), level (2), len (2)
    int totalBytes = ids.length * recordSize;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        // Write ID
        buffer[offset++] = (byte) id;
        buffer[offset++] = (byte) (id >>> 8);
        buffer[offset++] = (byte) (id >>> 16);
        buffer[offset++] = (byte) (id >>> 24);

        // Write TS
        buffer[offset++] = (byte) t;
        buffer[offset++] = (byte) (t >>> 8);
        buffer[offset++] = (byte) (t >>> 16);
        buffer[offset++] = (byte) (t >>> 24);

        // Write Level
        buffer[offset++] = (byte) lvl;
        buffer[offset++] = (byte) (lvl >>> 8);

        // Write Len
        buffer[offset++] = (byte) ln;
        buffer[offset++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}