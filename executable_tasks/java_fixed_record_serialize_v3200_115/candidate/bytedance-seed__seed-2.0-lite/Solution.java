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
        int offset = i * 12;
        // Write id in original little-endian order
        buffer[offset] = (byte) ids[i];
        buffer[offset + 1] = (byte) (ids[i] >>> 8);
        buffer[offset + 2] = (byte) (ids[i] >>> 16);
        buffer[offset + 3] = (byte) (ids[i] >>> 24);
        // Write timestamp
        buffer[offset + 4] = (byte) ts[i];
        buffer[offset + 5] = (byte) (ts[i] >>> 8);
        buffer[offset + 6] = (byte) (ts[i] >>> 16);
        buffer[offset + 7] = (byte) (ts[i] >>> 24);
        // Write levels
        buffer[offset + 8] = (byte) levels[i];
        buffer[offset + 9] = (byte) (levels[i] >>> 8);
        // Write lens
        buffer[offset + 10] = (byte) lens[i];
        buffer[offset + 11] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}