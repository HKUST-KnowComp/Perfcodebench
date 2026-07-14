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
    int recordSize = 12;
    int totalSize = ids.length * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        // Serialize ID (4 bytes)
        buffer[offset++] = (byte) id;
        buffer[offset++] = (byte) (id >>> 8);
        buffer[offset++] = (byte) (id >>> 16);
        buffer[offset++] = (byte) (id >>> 24);

        // Serialize TS (4 bytes)
        buffer[offset++] = (byte) t;
        buffer[offset++] = (byte) (t >>> 8);
        buffer[offset++] = (byte) (t >>> 16);
        buffer[offset++] = (byte) (t >>> 24);

        // Serialize Level (2 bytes)
        buffer[offset++] = (byte) lvl;
        buffer[offset++] = (byte) (lvl >>> 8);

        // Serialize Lens (2 bytes)
        buffer[offset++] = (byte) ln;
        buffer[offset++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}