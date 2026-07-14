import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int bufSize = n * 12;
    byte[] bytes = new byte[bufSize];
    ByteBuffer buf = ByteBuffer.wrap(bytes).order(ByteOrder.LITTLE_ENDIAN);
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      buf.clear();
      for (int i = 0; i < n; i++) {
        buf.putInt(ids[i]);
        buf.putInt(ts[i]);
        buf.putShort(levels[i]);
        buf.putShort(lens[i]);
      }
      long h = 1469598103934665603L;
      for (int i = 0; i < bufSize; i++) {
        h ^= (bytes[i] & 0xFFL);
        h *= 1099511628211L;
      }
      hash = h;
    }
    return hash;
  }
}
