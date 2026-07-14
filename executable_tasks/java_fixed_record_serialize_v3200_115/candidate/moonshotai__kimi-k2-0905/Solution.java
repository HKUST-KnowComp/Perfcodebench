import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data, int len) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < len; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int recSize = 12; // 4 + 4 + 2 + 2
    int total = ids.length * recSize;
    byte[] buf = new byte[total];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < ids.length; i++) {
        int v;
        v = ids[i];
        buf[pos++] = (byte) v;
        buf[pos++] = (byte) (v >>> 8);
        buf[pos++] = (byte) (v >>> 16);
        buf[pos++] = (byte) (v >>> 24);
        v = ts[i];
        buf[pos++] = (byte) v;
        buf[pos++] = (byte) (v >>> 8);
        buf[pos++] = (byte) (v >>> 16);
        buf[pos++] = (byte) (v >>> 24);
        short s;
        s = levels[i];
        buf[pos++] = (byte) s;
        buf[pos++] = (byte) (s >>> 8);
        s = lens[i];
        buf[pos++] = (byte) s;
        buf[pos++] = (byte) (s >>> 8);
      }
      hash = checksumBytes(buf, total);
    }
    return hash;
  }
}