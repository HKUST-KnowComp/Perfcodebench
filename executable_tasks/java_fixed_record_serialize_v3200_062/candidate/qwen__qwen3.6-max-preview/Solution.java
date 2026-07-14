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
    int n = ids.length;
    byte[] out = new byte[n * 12];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        out[idx++] = (byte) id;
        out[idx++] = (byte) (id >>> 8);
        out[idx++] = (byte) (id >>> 16);
        out[idx++] = (byte) (id >>> 24);
        int t = ts[i];
        out[idx++] = (byte) t;
        out[idx++] = (byte) (t >>> 8);
        out[idx++] = (byte) (t >>> 16);
        out[idx++] = (byte) (t >>> 24);
        short lv = levels[i];
        out[idx++] = (byte) lv;
        out[idx++] = (byte) (lv >>> 8);
        short ln = lens[i];
        out[idx++] = (byte) ln;
        out[idx++] = (byte) (ln >>> 8);
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}