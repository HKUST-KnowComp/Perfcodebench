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
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[idx] = (byte) id;
        buf[idx + 1] = (byte) (id >>> 8);
        buf[idx + 2] = (byte) (id >>> 16);
        buf[idx + 3] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[idx + 4] = (byte) t;
        buf[idx + 5] = (byte) (t >>> 8);
        buf[idx + 6] = (byte) (t >>> 16);
        buf[idx + 7] = (byte) (t >>> 24);
        
        short l = levels[i];
        buf[idx + 8] = (byte) l;
        buf[idx + 9] = (byte) (l >>> 8);
        
        short len = lens[i];
        buf[idx + 10] = (byte) len;
        buf[idx + 11] = (byte) (len >>> 8);
        
        idx += 12;
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}