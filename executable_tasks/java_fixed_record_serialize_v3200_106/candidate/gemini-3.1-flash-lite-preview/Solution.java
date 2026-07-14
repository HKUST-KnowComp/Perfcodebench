public class Solution {
  static long checksumBytes(byte[] data, int length) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int recordSize = 12;
    int totalSize = ids.length * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int offset = 0;
      for (int i = 0; i < ids.length; i++) {
        int id = ids[i];
        buffer[offset++] = (byte) id;
        buffer[offset++] = (byte) (id >>> 8);
        buffer[offset++] = (byte) (id >>> 16);
        buffer[offset++] = (byte) (id >>> 24);

        int t = ts[i];
        buffer[offset++] = (byte) t;
        buffer[offset++] = (byte) (t >>> 8);
        buffer[offset++] = (byte) (t >>> 16);
        buffer[offset++] = (byte) (t >>> 24);

        short level = levels[i];
        buffer[offset++] = (byte) level;
        buffer[offset++] = (byte) (level >>> 8);

        short len = lens[i];
        buffer[offset++] = (byte) len;
        buffer[offset++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer, totalSize);
    }
    return hash;
  }
}