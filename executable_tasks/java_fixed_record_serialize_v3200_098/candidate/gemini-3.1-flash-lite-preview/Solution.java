public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int recordSize = 12;
    int totalSize = ids.length * recordSize;
    byte[] buffer = new byte[totalSize];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      for (int i = 0; i < ids.length; i++) {
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

        short level = levels[i];
        buffer[pos++] = (byte) level;
        buffer[pos++] = (byte) (level >>> 8);

        short len = lens[i];
        buffer[pos++] = (byte) len;
        buffer[pos++] = (byte) (len >>> 8);
      }

      long h = 1469598103934665603L;
      for (int i = 0; i < totalSize; i++) {
        h ^= (buffer[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}