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
    final int recordBytes = 12;                 // 4 + 4 + 2 + 2
    final int totalBytes = ids.length * recordBytes;
    byte[] buffer = new byte[totalBytes];
    ByteBuffer bb = ByteBuffer.wrap(buffer);

    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      bb.clear();
      for (int i = 0; i < ids.length; i++) {
        bb.putInt(ids[i]);
        bb.putInt(ts[i]);
        bb.putShort(levels[i]);
        bb.putShort(lens[i]);
      }
      hash = checksumBytes(buffer);
    }
    return hash;
  }
}