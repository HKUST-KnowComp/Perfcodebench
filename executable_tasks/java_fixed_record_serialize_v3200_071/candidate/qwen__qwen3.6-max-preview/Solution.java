import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] buf = new byte[n * 12];
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
        
        short lv = levels[i];
        buf[idx++] = (byte) lv;
        buf[idx++] = (byte) (lv >>> 8);
        
        short ln = lens[i];
        buf[idx++] = (byte) ln;
        buf[idx++] = (byte) (ln >>> 8);
      }
      
      long h = 1469598103934665603L;
      for (int i = 0; i < buf.length; i++) {
        h ^= (buf[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}