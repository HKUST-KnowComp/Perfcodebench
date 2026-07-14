import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int bufLen = n * 12;
    byte[] buf = new byte[bufLen];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[pos]     = (byte) id;
        buf[pos + 1] = (byte) (id >>> 8);
        buf[pos + 2] = (byte) (id >>> 16);
        buf[pos + 3] = (byte) (id >>> 24);
        
        int t = ts[i];
        buf[pos + 4] = (byte) t;
        buf[pos + 5] = (byte) (t >>> 8);
        buf[pos + 6] = (byte) (t >>> 16);
        buf[pos + 7] = (byte) (t >>> 24);
        
        short lv = levels[i];
        buf[pos + 8] = (byte) lv;
        buf[pos + 9] = (byte) (lv >>> 8);
        
        short ln = lens[i];
        buf[pos + 10] = (byte) ln;
        buf[pos + 11] = (byte) (ln >>> 8);
        
        pos += 12;
      }
      
      long h = 1469598103934665603L;
      int i = 0;
      int limit = bufLen - 7;
      while (i < limit) {
        h ^= (buf[i] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 1] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 2] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 3] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 4] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 5] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 6] & 0xFFL);
        h *= 1099511628211L;
        h ^= (buf[i + 7] & 0xFFL);
        h *= 1099511628211L;
        i += 8;
      }
      while (i < bufLen) {
        h ^= (buf[i] & 0xFFL);
        h *= 1099511628211L;
        i++;
      }
      hash = h;
    }
    return hash;
  }
}