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
    int recordCount = ids.length;
    int totalBytes = 12 * recordCount;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < recordCount; i++) {
        int pos = i * 12;
        // Serialize ID (4 bytes, same order as original implementation)
        int id = ids[i];
        buffer[pos] = (byte) id;
        buffer[pos+1] = (byte) (id >>> 8);
        buffer[pos+2] = (byte) (id >>> 16);
        buffer[pos+3] = (byte) (id >>> 24);
        // Serialize timestamp (4 bytes)
        int t = ts[i];
        buffer[pos+4] = (byte) t;
        buffer[pos+5] = (byte) (t >>> 8);
        buffer[pos+6] = (byte) (t >>> 16);
        buffer[pos+7] = (byte) (t >>> 24);
        // Serialize level (2 bytes)
        short lvl = levels[i];
        buffer[pos+8] = (byte) lvl;
        buffer[pos+9] = (byte) (lvl >>> 8);
        // Serialize length (2 bytes)
        short len = lens[i];
        buffer[pos+10] = (byte) len;
        buffer[pos+11] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}