import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int recordSize = 12; // 4 + 4 + 2 + 2 bytes per record
    byte[] buf = new byte[n * recordSize];
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
        
        short lv = levels[i];
        buf[pos++] = (byte) lv;
        buf[pos++] = (byte) (lv >>> 8);
        
        short ln = lens[i];
        buf[pos++] = (byte) ln;
        buf[pos++] = (byte) (ln >>> 8);
      }
      
      long h = 1469598103934665603L;
      for (int i = 0; i < pos; i++) {
        h ^= (buf[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}