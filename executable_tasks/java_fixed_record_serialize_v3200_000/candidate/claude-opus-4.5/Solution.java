import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int recordSize = 12; // 4 + 4 + 2 + 2
    int totalSize = n * recordSize;
    byte[] bytes = new byte[totalSize];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        bytes[pos]     = (byte) id;
        bytes[pos + 1] = (byte) (id >>> 8);
        bytes[pos + 2] = (byte) (id >>> 16);
        bytes[pos + 3] = (byte) (id >>> 24);
        
        int t = ts[i];
        bytes[pos + 4] = (byte) t;
        bytes[pos + 5] = (byte) (t >>> 8);
        bytes[pos + 6] = (byte) (t >>> 16);
        bytes[pos + 7] = (byte) (t >>> 24);
        
        short lv = levels[i];
        bytes[pos + 8] = (byte) lv;
        bytes[pos + 9] = (byte) (lv >>> 8);
        
        short ln = lens[i];
        bytes[pos + 10] = (byte) ln;
        bytes[pos + 11] = (byte) (ln >>> 8);
        
        pos += 12;
      }
      
      // FNV-1a checksum
      long h = 1469598103934665603L;
      int i = 0;
      int limit = totalSize - 7;
      while (i < limit) {
        h ^= (bytes[i] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 1] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 2] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 3] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 4] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 5] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 6] & 0xFFL);
        h *= 1099511628211L;
        h ^= (bytes[i + 7] & 0xFFL);
        h *= 1099511628211L;
        i += 8;
      }
      while (i < totalSize) {
        h ^= (bytes[i] & 0xFFL);
        h *= 1099511628211L;
        i++;
      }
      hash = h;
    }
    return hash;
  }
}