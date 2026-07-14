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
    if (iters <= 0) {
      return 0L;
    }
    int n = ids.length;
    byte[] bytes = new byte[n * 12];
    int p = 0;
    for (int i = 0; i < n; i++) {
      int id = ids[i];
      bytes[p++] = (byte) id;
      bytes[p++] = (byte) (id >>> 8);
      bytes[p++] = (byte) (id >>> 16);
      bytes[p++] = (byte) (id >>> 24);
      int t = ts[i];
      bytes[p++] = (byte) t;
      bytes[p++] = (byte) (t >>> 8);
      bytes[p++] = (byte) (t >>> 16);
      bytes[p++] = (byte) (t >>> 24);
      short lv = levels[i];
      bytes[p++] = (byte) lv;
      bytes[p++] = (byte) (lv >>> 8);
      short ln = lens[i];
      bytes[p++] = (byte) ln;
      bytes[p++] = (byte) (ln >>> 8);
    }
    return checksumBytes(bytes);
  }
}
