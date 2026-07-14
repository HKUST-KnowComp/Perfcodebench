import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  static long checksumBytes(byte[] data) {
    long hash = FNV_OFFSET;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= FNV_PRIME;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int n = ids.length;
    int totalBytes = n * 12;
    byte[] buf = new byte[totalBytes];
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
      hash = checksumBytes(buf);
    }
    return hash;
  }
}