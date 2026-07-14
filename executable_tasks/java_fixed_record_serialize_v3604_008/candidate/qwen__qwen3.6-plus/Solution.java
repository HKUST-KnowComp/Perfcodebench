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
    int size = n * 12;
    byte[] buf = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[idx++] = (byte) id;
        buf[idx++] = (byte) (id >>> 8);
        buf[idx++] = (byte) (id >>> 16);
        buf[idx++] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[idx++] = (byte) t;
        buf[idx++] = (byte) (t >>> 8);
        buf[idx++] = (byte) (t >>> 16);
        buf[idx++] = (byte) (t >>> 24);
        
        short l = levels[i];
        buf[idx++] = (byte) l;
        buf[idx++] = (byte) (l >>> 8);
        
        short len = lens[i];
        buf[idx++] = (byte) len;
        buf[idx++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}