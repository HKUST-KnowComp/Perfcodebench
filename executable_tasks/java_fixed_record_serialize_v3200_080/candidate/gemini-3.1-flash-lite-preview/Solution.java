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
    byte[] buffer = new byte[n * 12];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);
        int t = ts[i];
        buffer[pos++] = (byte) t;
        buffer[pos++] = (byte) (t >>> 8);
        buffer[pos++] = (byte) (t >>> 16);
        buffer[pos++] = (byte) (t >>> 24);
        short l1 = levels[i];
        buffer[pos++] = (byte) l1;
        buffer[pos++] = (byte) (l1 >>> 8);
        short l2 = lens[i];
        buffer[pos++] = (byte) l2;
        buffer[pos++] = (byte) (l2 >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}