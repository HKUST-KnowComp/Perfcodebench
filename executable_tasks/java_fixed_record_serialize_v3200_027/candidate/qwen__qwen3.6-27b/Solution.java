import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] buf = new byte[n * 12];
    long hash = 0;
    int bufLen = buf.length;
    for (int iter = 0; iter < iters; iter++) {
      int off = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[off] = (byte) id;
        buf[off + 1] = (byte) (id >>> 8);
        buf[off + 2] = (byte) (id >>> 16);
        buf[off + 3] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[off + 4] = (byte) t;
        buf[off + 5] = (byte) (t >>> 8);
        buf[off + 6] = (byte) (t >>> 16);
        buf[off + 7] = (byte) (t >>> 24);
        
        short lv = levels[i];
        buf[off + 8] = (byte) lv;
        buf[off + 9] = (byte) (lv >>> 8);
        
        short ln = lens[i];
        buf[off + 10] = (byte) ln;
        buf[off + 11] = (byte) (ln >>> 8);
        
        off += 12;
      }
      
      long h = 1469598103934665603L;
      for (int i = 0; i < bufLen; i++) {
        h ^= (buf[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}