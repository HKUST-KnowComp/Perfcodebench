import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Solution {
  private static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    byte[] bytes = new byte[n * 12];
    ByteBuffer buf = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);
    for (int i = 0; i < n; i++) {
      buf.putInt(ids[i]);
      buf.putInt(ts[i]);
      buf.putShort(levels[i]);
      buf.putShort(lens[i]);
    }
    return checksumBytes(bytes);
  }
}