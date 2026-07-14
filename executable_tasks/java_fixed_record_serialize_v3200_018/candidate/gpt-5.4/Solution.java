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
    final byte[] out = new byte[n * recordSize];
    long hash = 0L;

    for (int iter = 0; iter < iters; iter++) {
      int p = 0;
      for (int i = 0; i < n; i++) {
        int v = ids[i];
        out[p++] = (byte) v;
        out[p++] = (byte) (v >>> 8);
        out[p++] = (byte) (v >>> 16);
        out[p++] = (byte) (v >>> 24);

        v = ts[i];
        out[p++] = (byte) v;
        out[p++] = (byte) (v >>> 8);
        out[p++] = (byte) (v >>> 16);
        out[p++] = (byte) (v >>> 24);

        int s = levels[i];
        out[p++] = (byte) s;
        out[p++] = (byte) (s >>> 8);

        s = lens[i];
        out[p++] = (byte) s;
        out[p++] = (byte) (s >>> 8);
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}
