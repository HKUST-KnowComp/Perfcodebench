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
    if (ids.length == 0) return 0;
    final int recordSize = 12;
    final int totalBytes = ids.length * recordSize;
    final byte[] buf = new byte[totalBytes];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < ids.length; i++) {
        final int offset = i * recordSize;
        // Write 4-byte id, matching original byte order
        final int id = ids[i];
        buf[offset] = (byte) id;
        buf[offset + 1] = (byte) (id >>> 8);
        buf[offset + 2] = (byte) (id >>> 16);
        buf[offset + 3] = (byte) (id >>> 24);
        // Write 4-byte timestamp
        final int t = ts[i];
        buf[offset + 4] = (byte) t;
        buf[offset + 5] = (byte) (t >>> 8);
        buf[offset + 6] = (byte) (t >>> 16);
        buf[offset + 7] = (byte) (t >>> 24);
        // Write 2-byte level
        final short lvl = levels[i];
        buf[offset + 8] = (byte) lvl;
        buf[offset + 9] = (byte) (lvl >>> 8);
        // Write 2-byte length
        final short len = lens[i];
        buf[offset + 10] = (byte) len;
        buf[offset + 11] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}