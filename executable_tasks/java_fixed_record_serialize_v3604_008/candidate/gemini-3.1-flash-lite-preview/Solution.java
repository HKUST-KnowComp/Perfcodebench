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
    int recordSize = 16;
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
        
        // Padding to match original 16-byte structure if needed, 
        // though original code added 12 bytes per record. 
        // Adjusting to match original logic exactly:
        pos += 4; 
      }
      hash = checksumBytes(buffer, totalSize);
    }
    return hash;
  }
}