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
      int off = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[off++] = (byte) id;
        buf[off++] = (byte) (id >>> 8);
        buf[off++] = (byte) (id >>> 16);
        buf[off++] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[off++] = (byte) t;
        buf[off++] = (byte) (t >>> 8);
        buf[off++] = (byte) (t >>> 16);
        buf[off++] = (byte) (t >>> 24);
        
        short l = levels[i];
        buf[off++] = (byte) l;
        buf[off++] = (byte) (l >>> 8);
        
        short ln = lens[i];
        buf[off++] = (byte) ln;
        buf[off++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}
