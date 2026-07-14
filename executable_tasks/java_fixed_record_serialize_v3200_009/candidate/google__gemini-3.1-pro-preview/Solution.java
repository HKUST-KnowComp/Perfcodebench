import java.util.*;

public class Solution {
  /**
   * Computes the FNV-1a 64-bit hash of the byte array.
   */
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  /**
   * Serializes fixed-width records into a byte array and returns the checksum of the last iteration.
   * Record structure (12 bytes total):
   * - int ids[i] (4 bytes, little-endian)
   * - int ts[i] (4 bytes, little-endian)
   * - short levels[i] (2 bytes, little-endian)
   * - short lens[i] (2 bytes, little-endian)
   */
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int recordSize = 12;
    byte[] buffer = new byte[n * recordSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < n; i++) {
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

        short level = levels[i];
        buffer[offset++] = (byte) level;
        buffer[offset++] = (byte) (level >>> 8);

        short len = lens[i];
        buffer[offset++] = (byte) len;
        buffer[offset++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}