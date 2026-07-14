import java.util.*;

public class Solution {
  /**
   * Optimized serialization and checksum calculation.
   * The baseline used ArrayList<Byte> which causes massive boxing overhead and multiple copies.
   * We pre-calculate the required buffer size and perform the FNV-1a hash directly.
   */
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int recordSize = 12; // 4 (id) + 4 (ts) + 2 (level) + 2 (len)
    byte[] buffer = new byte[n * recordSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
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

        short level = levels[i];
        buffer[pos++] = (byte) level;
        buffer[pos++] = (byte) (level >>> 8);

        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }
      
      // Calculate FNV-1a hash
      long h = 1469598103934665603L;
      for (int i = 0; i < buffer.length; i++) {
        h ^= (buffer[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}