java
import java.nio.ByteBuffer;

public class Solution {
  public static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int totalSize = ids.length * 12;
    ByteBuffer buffer = ByteBuffer.allocate(totalSize);
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      buffer.clear();
      for (int i = 0; i < ids.length; i++) {
        buffer.putInt(ids[i]);
        buffer.putInt(ts[i]);
        buffer.putShort(levels[i]);
        buffer.putShort(lens[i]);
      }
      hash = checksumBytes(buffer.array());
    }
    return hash;
  }
}