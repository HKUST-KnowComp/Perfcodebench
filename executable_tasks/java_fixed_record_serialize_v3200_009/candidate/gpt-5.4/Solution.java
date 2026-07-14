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
    final int recordSize = 12;
    final byte[] bytes = new byte[n * recordSize];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int p = 0;
      for (int i = 0; i < n; i++) {
        final int id = ids[i];
        bytes[p++] = (byte) id;
        bytes[p++] = (byte) (id >>> 8);
        bytes[p++] = (byte) (id >>> 16);
        bytes[p++] = (byte) (id >>> 24);

        final int t = ts[i];
        bytes[p++] = (byte) t;
        bytes[p++] = (byte) (t >>> 8);
        bytes[p++] = (byte) (t >>> 16);
        bytes[p++] = (byte) (t >>> 24);

        final int level = levels[i] & 0xFFFF;
        bytes[p++] = (byte) level;
        bytes[p++] = (byte) (level >>> 8);

        final int len = lens[i] & 0xFFFF;
        bytes[p++] = (byte) len;
        bytes[p++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}
