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
    final int recordBytes = 12;                 // 4 + 4 + 2 + 2
    final int totalBytes = ids.length * recordBytes;
    byte[] buf = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < ids.length; i++) {
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

        short lvl = levels[i];
        buf[pos++] = (byte) lvl;
        buf[pos++] = (byte) (lvl >>> 8);

        short ln = lens[i];
        buf[pos++] = (byte) ln;
        buf[pos++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(buf);
    }
    return hash;
  }
}