java
import java.nio.ByteBuffer;
import java.util.Arrays;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int recordSize = 12;
    int totalSize = ids.length * recordSize;
    ByteBuffer buffer = ByteBuffer.allocate(totalSize);
    for (int iter = 0; iter < iters; iter++) {
      buffer.clear();
      for (int i = 0; i < ids.length; i++) {
        buffer.put((byte) (ids[i]));
        buffer.put((byte) (ids[i] >>> 8));
        buffer.put((byte) (ids[i] >>> 16));
        buffer.put((byte) (ids[i] >>> 24));
        buffer.put((byte) (ts[i]));
        buffer.put((byte) (ts[i] >>> 8));
        buffer.put((byte) (ts[i] >>> 16));
        buffer.put((byte) (ts[i] >>> 24));
        buffer.putShort(levels[i]);
        buffer.putShort(lens[i]);
      }
      byte[] bytes = buffer.array();
      long hash = checksumBytes(bytes);
    }
    return hash;
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }
}