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
    if (ids.length == 0) {
      long hash = 0;
      for (int i = 0; i < iters; i++) {
        hash = checksumBytes(new byte[0]);
      }
      return hash;
    }
    final int recordSize = 12;
    final byte[] out = new byte[ids.length * recordSize];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < ids.length; i++) {
        final int off = i * recordSize;
        int id = ids[i];
        out[off] = (byte) id;
        out[off + 1] = (byte) (id >>> 8);
        out[off + 2] = (byte) (id >>> 16);
        out[off + 3] = (byte) (id >>> 24);
        int t = ts[i];
        out[off + 4] = (byte) t;
        out[off + 5] = (byte) (t >>> 8);
        out[off + 6] = (byte) (t >>> 16);
        out[off + 7] = (byte) (t >>> 24);
        short lvl = levels[i];
        out[off + 8] = (byte) lvl;
        out[off + 9] = (byte) (lvl >>> 8);
        short len = lens[i];
        out[off + 10] = (byte) len;
        out[off + 11] = (byte) (len >>> 8);
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}