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
    int totalBytes = recordCount * 12;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < recordCount; i++) {
        // Write 4-byte id (little-endian, matches original order)
        int id = ids[i];
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);
        
        // Write 4-byte timestamp
        int time = ts[i];
        buffer[pos++] = (byte) time;
        buffer[pos++] = (byte) (time >>> 8);
        buffer[pos++] = (byte) (time >>> 16);
        buffer[pos++] = (byte) (time >>> 24);
        
        // Write 2-byte level
        short lvl = levels[i];
        buffer[pos++] = (byte) lvl;
        buffer[pos++] = (byte) (lvl >>> 8);
        
        // Write 2-byte length
        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}