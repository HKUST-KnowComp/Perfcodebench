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
    int totalSize = ids.length * 12;
    byte[] bytes = new byte[totalSize];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < ids.length; i++) {
        int base = i * 12;
        bytes[base] = (byte) ids[i];
        bytes[base+1] = (byte) (ids[i] >>> 8);
        bytes[base+2] = (byte) (ids[i] >>> 16);
        bytes[base+3] = (byte) (ids[i] >>> 24);
        bytes[base+4] = (byte) ts[i];
        bytes[base+5] = (byte) (ts[i] >>> 8);
        bytes[base+6] = (byte) (ts[i] >>> 16);
        bytes[base+7] = (byte) (ts[i] >>> 24);
        bytes[base+8] = (byte) levels[i];
        bytes[base+9] = (byte) (levels[i] >>> 8);
        bytes[base+10] = (byte) lens[i];
        bytes[base+11] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}