import java.nio.ByteBuffer;

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
    int recordBytes = ids.length * 12;           // 4 + 4 + 2 + 2
    ByteBuffer buf = ByteBuffer.allocate(recordBytes);
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      buf.clear();
      for (int i = 0; i < ids.length; i++) {
        buf.putInt(ids[i]);
        buf.putInt(ts[i]);
        buf.putShort(levels[i]);
        buf.putShort(lens[i]);
      }
      hash = checksumBytes(buf.array());
    }
    return hash;
  }
}