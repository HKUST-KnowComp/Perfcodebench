import java.util.*;

public class Solution {
  /**
   * Optimizes the serialization and checksum process.
   * Instead of using ArrayList<Byte> (which involves boxing and overhead),
   * we use a pre-allocated byte array and compute the FNV-1a hash directly.
   */
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    // Each record is 4 (id) + 4 (ts) + 2 (level) + 2 (len) = 12 bytes.
    int totalBytes = n * 12;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      // Serialize all records into the buffer
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buffer[idx++] = (byte) id;
        buffer[idx++] = (byte) (id >>> 8);
        buffer[idx++] = (byte) (id >>> 16);
        buffer[idx++] = (byte) (id >>> 24);

        int t = ts[i];
        buffer[idx++] = (byte) t;
        buffer[idx++] = (byte) (t >>> 8);
        buffer[idx++] = (byte) (t >>> 16);
        buffer[idx++] = (byte) (t >>> 24);

        short level = levels[i];
        buffer[idx++] = (byte) level;
        buffer[idx++] = (byte) (level >>> 8);

        short len = lens[i];
        buffer[idx++] = (byte) len;
        buffer[idx++] = (byte) (len >>> 8);
      }

      // Compute FNV-1a hash of the buffer
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