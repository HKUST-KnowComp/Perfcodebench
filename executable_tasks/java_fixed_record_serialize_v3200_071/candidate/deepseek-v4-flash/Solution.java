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
    if (iters <= 0) {
      return 0L;
    }
    int n = ids.length;
    int recordSize = 12;
    byte[] buffer = new byte[n * recordSize];
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
    long hash = 0L;
    for (int iter = 0; iter < iters; iter++) {
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}