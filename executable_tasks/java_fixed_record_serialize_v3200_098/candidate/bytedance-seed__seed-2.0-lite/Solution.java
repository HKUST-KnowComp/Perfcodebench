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
    final int recordCount = ids.length;
    final int totalBytes = recordCount * 12;
    final byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < recordCount; i++) {
        final int offset = i * 12;
        // Write id in identical little-endian order to original
        final int id = ids[i];
        buffer[offset] = (byte) id;
        buffer[offset + 1] = (byte) (id >>> 8);
        buffer[offset + 2] = (byte) (id >>> 16);
        buffer[offset + 3] = (byte) (id >>> 24);
        // Write timestamp
        final int t = ts[i];
        buffer[offset + 4] = (byte) t;
        buffer[offset + 5] = (byte) (t >>> 8);
        buffer[offset + 6] = (byte) (t >>> 16);
        buffer[offset + 7] = (byte) (t >>> 24);
        // Write level
        final short lvl = levels[i];
        buffer[offset + 8] = (byte) lvl;
        buffer[offset + 9] = (byte) (lvl >>> 8);
        // Write length
        final short len = lens[i];
        buffer[offset + 10] = (byte) len;
        buffer[offset + 11] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}