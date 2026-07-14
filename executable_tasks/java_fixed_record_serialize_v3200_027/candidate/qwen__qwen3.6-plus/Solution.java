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
    int n = ids.length;
    byte[] buf = new byte[n * 12];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[pos++] = (byte) id;
        buf[pos++] = (byte) (id >>> 8);
        buf[pos++] = (byte) (id >>> 16);
        buf[pos++] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[pos++] = (byte) t;
        buf[pos++] = (byte) (t >>> 8);
        buf[pos++] = (byte) (t >>> 16);
        buf[pos++] = (byte) (t >>> 24);
        
        short l = levels[i];
        buf[pos++] = (byte) l;
        buf[pos++] = (byte) (l >>> 8);
        
        short len = lens[i];
        buf[pos++] = (byte) len;
        buf[pos++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}