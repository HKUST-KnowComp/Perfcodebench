import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    // Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes
    int recordSize = 12;
    int totalSize = n * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lv = levels[i];
        short ln = lens[i];

        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);
        buffer[pos++] = (byte) t;
        buffer[pos++] = (byte) (t >>> 8);
        buffer[pos++] = (byte) (t >>> 16);
        buffer[pos++] = (byte) (t >>> 24);
        buffer[pos++] = (byte) lv;
        buffer[pos++] = (byte) (lv >>> 8);
        buffer[pos++] = (byte) ln;
        buffer[pos++] = (byte) (ln >>> 8);
      }

      // Calculate checksum of the buffer
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