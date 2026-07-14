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
    int recordCount = ids.length;
    int totalBytes = recordCount * 12;
    byte[] buf = new byte[totalBytes];
    long hash = 0;

    for (int i = 0; i < recordCount; i++) {
      int pos = i * 12;
      int id = ids[i];
      buf[pos] = (byte) id;
      buf[pos+1] = (byte) (id >>> 8);
      buf[pos+2] = (byte) (id >>> 16);
      buf[pos+3] = (byte) (id >>> 24);
      
      int t = ts[i];
      buf[pos+4] = (byte) t;
      buf[pos+5] = (byte) (t >>> 8);
      buf[pos+6] = (byte) (t >>> 16);
      buf[pos+7] = (byte) (t >>> 24);
      
      short lvl = levels[i];
      buf[pos+8] = (byte) lvl;
      buf[pos+9] = (byte) (lvl >>> 8);
      
      short len = lens[i];
      buf[pos+10] = (byte) len;
      buf[pos+11] = (byte) (len >>> 8);
    }

    for (int iter = 0; iter < iters; iter++) {
      hash = checksumBytes(buf);
    }
    return hash;
  }
}