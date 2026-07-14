import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] buf = new byte[n * 12];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[offset++] = (byte) id;
        buf[offset++] = (byte) (id >>> 8);
        buf[offset++] = (byte) (id >>> 16);
        buf[offset++] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[offset++] = (byte) t;
        buf[offset++] = (byte) (t >>> 8);
        buf[offset++] = (byte) (t >>> 16);
        buf[offset++] = (byte) (t >>> 24);
        
        int lv = levels[i];
        buf[offset++] = (byte) lv;
        buf[offset++] = (byte) (lv >>> 8);
        
        int ln = lens[i];
        buf[offset++] = (byte) ln;
        buf[offset++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}