import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int recordSize = 4 + 4 + 2 + 2; // 12 bytes per record
    int totalBytes = ids.length * recordSize;
    byte[] buffer = new byte[totalBytes];
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

        short len = lens[i];
        buffer[offset++] = (byte) len;
        buffer[offset++] = (byte) (len >>> 8);
      }

      // Inline checksumBytes to avoid extra method call overhead and use the buffer
      long currentHash = 1469598103934665603L;
      for (int i = 0; i < totalBytes; i++) {
        currentHash ^= (buffer[i] & 0xFFL);
        currentHash *= 1099511628211L;
      }
      hash = currentHash;
    }
    return hash;
  }
}