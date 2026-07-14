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
    int size = ids.length * 12;
    byte[] bytes = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < ids.length; i++) {
        int offset = i * 12;
        bytes[offset] = (byte) ids[i];
        bytes[offset+1] = (byte) (ids[i] >>> 8);
        bytes[offset+2] = (byte) (ids[i] >>> 16);
        bytes[offset+3] = (byte) (ids[i] >>> 24);
        bytes[offset+4] = (byte) ts[i];
        bytes[offset+5] = (byte) (ts[i] >>> 8);
        bytes[offset+6] = (byte) (ts[i] >>> 16);
        bytes[offset+7] = (byte) (ts[i] >>> 24);
        bytes[offset+8] = (byte) levels[i];
        bytes[offset+9] = (byte) (levels[i] >>> 8);
        bytes[offset+10] = (byte) lens[i];
        bytes[offset+11] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}