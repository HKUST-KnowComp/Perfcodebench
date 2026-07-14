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
    final int RECORD_BYTES = 12;
    byte[] buffer = new byte[n * RECORD_BYTES];
    long finalHash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        // Write ID in exact same byte order as original implementation
        int id = ids[i];
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);
        
        // Write timestamp
        int timestamp = ts[i];
        buffer[pos++] = (byte) timestamp;
        buffer[pos++] = (byte) (timestamp >>> 8);
        buffer[pos++] = (byte) (timestamp >>> 16);
        buffer[pos++] = (byte) (timestamp >>> 24);
        
        // Write level
        short level = levels[i];
        buffer[pos++] = (byte) level;
        buffer[pos++] = (byte) (level >>> 8);
        
        // Write length
        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }
      finalHash = checksumBytes(buffer);
    }
    return finalHash;
  }
}