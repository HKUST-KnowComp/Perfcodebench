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
    byte[] buf = new byte[recordCount * 12];
    
    // Fill buffer once with fixed input data to avoid redundant work
    for (int i = 0; i < recordCount; i++) {
      int pos = i * 12;
      // Write id (4 bytes, identical order to original implementation)
      buf[pos] = (byte) ids[i];
      buf[pos+1] = (byte) (ids[i] >>> 8);
      buf[pos+2] = (byte) (ids[i] >>> 16);
      buf[pos+3] = (byte) (ids[i] >>> 24);
      // Write timestamp (4 bytes)
      buf[pos+4] = (byte) ts[i];
      buf[pos+5] = (byte) (ts[i] >>> 8);
      buf[pos+6] = (byte) (ts[i] >>> 16);
      buf[pos+7] = (byte) (ts[i] >>> 24);
      // Write level (2 bytes)
      buf[pos+8] = (byte) levels[i];
      buf[pos+9] = (byte) (levels[i] >>> 8);
      // Write length (2 bytes)
      buf[pos+10] = (byte) lens[i];
      buf[pos+11] = (byte) (lens[i] >>> 8);
    }
    
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      hash = checksumBytes(buf);
    }
    return hash;
  }
}