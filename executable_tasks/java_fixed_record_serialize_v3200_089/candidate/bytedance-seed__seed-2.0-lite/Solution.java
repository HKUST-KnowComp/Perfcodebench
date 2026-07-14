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
    int recordCount = ids.length;
    int totalBytes = recordCount * 12;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < recordCount; i++) {
        // Write id (4 bytes, same order as original)
        int id = ids[i];
        buffer[pos++] = (byte) id;
        buffer[pos++] = (byte) (id >>> 8);
        buffer[pos++] = (byte) (id >>> 16);
        buffer[pos++] = (byte) (id >>> 24);

        // Write ts (4 bytes, same order as original)
        int timestamp = ts[i];
        buffer[pos++] = (byte) timestamp;
        buffer[pos++] = (byte) (timestamp >>> 8);
        buffer[pos++] = (byte) (timestamp >>> 16);
        buffer[pos++] = (byte) (timestamp >>> 24);

        // Write levels (2 bytes, same order as original)
        short level = levels[i];
        buffer[pos++] = (byte) level;
        buffer[pos++] = (byte) (level >>> 8);

        // Write lens (2 bytes, same order as original)
        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}