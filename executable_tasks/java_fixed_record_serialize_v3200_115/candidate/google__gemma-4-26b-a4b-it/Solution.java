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
    int n = ids.length;
    int recordSize = 4 + 4 + 2 + 2; // id (4), ts (4), level (2), len (2)
    byte[] buffer = new byte[n * recordSize];
    long lastHash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short lvl = levels[i];
        short ln = lens[i];

        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);

        buffer[pos++] = (byte) t;
        buffer[pos++] = (byte) (t >>> 8);
        buffer[pos++] = (byte) (t >>> 16);
        buffer[pos++] = (byte) (t >>> 24);

        buffer[pos++] = (byte) lvl;
        buffer[pos++] = (byte) (lvl >>> 8);

        buffer[pos++] = (byte) ln;
        buffer[pos++] = (byte) (ln >>> 8);
      }
      
      // The requirement asks for the checksum of the serialized byte buffer.
      // We must use the exact same checksum algorithm as the baseline.
      lastHash = checksumBytes(buffer);
    }
    return lastHash;
  }
}