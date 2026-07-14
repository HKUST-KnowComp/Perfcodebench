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
    if (iters == 0) {
      return 0;
    }
    int n = ids.length;
    byte[] bytes = new byte[n * 12];
    int off = 0;
    for (int i = 0; i < n; i++) {
      int id = ids[i];
      bytes[off++] = (byte) id;
      bytes[off++] = (byte) (id >>> 8);
      bytes[off++] = (byte) (id >>> 16);
      bytes[off++] = (byte) (id >>> 24);
      int t = ts[i];
      bytes[off++] = (byte) t;
      bytes[off++] = (byte) (t >>> 8);
      bytes[off++] = (byte) (t >>> 16);
      bytes[off++] = (byte) (t >>> 24);
      short lv = levels[i];
      bytes[off++] = (byte) lv;
      bytes[off++] = (byte) (lv >>> 8);
      short ln = lens[i];
      bytes[off++] = (byte) ln;
      bytes[off++] = (byte) (ln >>> 8);
    }
    return checksumBytes(bytes);
  }
}