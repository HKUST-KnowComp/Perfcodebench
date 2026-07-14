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
    int recordSize = 12; // 4+4+2+2 bytes per record
    int totalSize = ids.length * recordSize;
    byte[] bytes = new byte[totalSize];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      for (int i = 0; i < ids.length; i++) {
        bytes[idx++] = (byte) ids[i];
        bytes[idx++] = (byte) (ids[i] >>> 8);
        bytes[idx++] = (byte) (ids[i] >>> 16);
        bytes[idx++] = (byte) (ids[i] >>> 24);
        bytes[idx++] = (byte) ts[i];
        bytes[idx++] = (byte) (ts[i] >>> 8);
        bytes[idx++] = (byte) (ts[i] >>> 16);
        bytes[idx++] = (byte) (ts[i] >>> 24);
        bytes[idx++] = (byte) levels[i];
        bytes[idx++] = (byte) (levels[i] >>> 8);
        bytes[idx++] = (byte) lens[i];
        bytes[idx++] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}