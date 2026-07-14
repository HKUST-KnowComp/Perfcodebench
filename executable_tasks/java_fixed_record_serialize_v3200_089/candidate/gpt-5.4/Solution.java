import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    final int n = ids.length;
    final int totalBytes = n * 12;
    final byte[] out = new byte[totalBytes];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int p = 0;
      for (int i = 0; i < n; i++) {
        final int id = ids[i];
        out[p++] = (byte) id;
        out[p++] = (byte) (id >>> 8);
        out[p++] = (byte) (id >>> 16);
        out[p++] = (byte) (id >>> 24);

        final int t = ts[i];
        out[p++] = (byte) t;
        out[p++] = (byte) (t >>> 8);
        out[p++] = (byte) (t >>> 16);
        out[p++] = (byte) (t >>> 24);

        final int level = levels[i] & 0xFFFF;
        out[p++] = (byte) level;
        out[p++] = (byte) (level >>> 8);

        final int len = lens[i] & 0xFFFF;
        out[p++] = (byte) len;
        out[p++] = (byte) (len >>> 8);
      }

      long h = 1469598103934665603L;
      for (int i = 0; i < totalBytes; i++) {
        h ^= (out[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }

    return hash;
  }
}
