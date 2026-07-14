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
    int recordSize = 4 + 4 + 2 + 2; // 12 bytes per record
    int totalSize = ids.length * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);

        int t = ts[i];
        buffer[pos++] = (byte) t;
        buffer[pos++] = (byte) (t >>> 8);
        buffer[pos++] = (byte) (t >>> 16);
        buffer[pos++] = (byte) (t >>> 24);

        short lvl = levels[i];
        buffer[pos++] = (byte) lvl;
        buffer[pos++] = (byte) (lvl >>> 8);

        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }
      
      // Calculate checksum on the filled buffer
      long currentHash = 1469598103934665603L;
      for (int i = 0; i < totalSize; i++) {
        currentHash ^= (buffer[i] & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }
    return hash;
  }
}