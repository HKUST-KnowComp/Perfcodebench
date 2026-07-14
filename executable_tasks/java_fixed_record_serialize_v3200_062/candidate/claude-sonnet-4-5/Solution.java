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
    int recordSize = 12;
    int totalSize = ids.length * recordSize;
    byte[] bytes = new byte[totalSize];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        bytes[pos++] = (byte) id;
        bytes[pos++] = (byte) (id >>> 8);
        bytes[pos++] = (byte) (id >>> 16);
        bytes[pos++] = (byte) (id >>> 24);
        
        int t = ts[i];
        bytes[pos++] = (byte) t;
        bytes[pos++] = (byte) (t >>> 8);
        bytes[pos++] = (byte) (t >>> 16);
        bytes[pos++] = (byte) (t >>> 24);
        
        short level = levels[i];
        bytes[pos++] = (byte) level;
        bytes[pos++] = (byte) (level >>> 8);
        
        short len = lens[i];
        bytes[pos++] = (byte) len;
        bytes[pos++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}