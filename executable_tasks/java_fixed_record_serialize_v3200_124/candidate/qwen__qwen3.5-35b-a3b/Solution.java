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
      int index = 0;
      for (int i = 0; i < ids.length; i++) {
        bytes[index++] = (byte) ids[i];
        bytes[index++] = (byte) (ids[i] >>> 8);
        bytes[index++] = (byte) (ids[i] >>> 16);
        bytes[index++] = (byte) (ids[i] >>> 24);
        bytes[index++] = (byte) ts[i];
        bytes[index++] = (byte) (ts[i] >>> 8);
        bytes[index++] = (byte) (ts[i] >>> 16);
        bytes[index++] = (byte) (ts[i] >>> 24);
        bytes[index++] = (byte) levels[i];
        bytes[index++] = (byte) (levels[i] >>> 8);
        bytes[index++] = (byte) lens[i];
        bytes[index++] = (byte) (lens[i] >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}