import java.io.ByteArrayOutputStream; import java.io.DataOutputStream; import java.nio.ByteBuffer; import java.nio.ByteOrder;

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
    int totalSize = ids.length * (4 + 4 + 2 + 2);
    ByteArrayOutputStream bos = new ByteArrayOutputStream(totalSize);
    DataOutputStream dos = new DataOutputStream(bos);
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      try {
        bos.reset();
        for (int i = 0; i < ids.length; i++) {
          dos.writeInt(ids[i]);
          dos.writeInt(ts[i]);
          dos.writeShort(levels[i]);
          dos.writeShort(lens[i]);
        }
        hash = checksumBytes(bos.toByteArray());
      } catch (Exception e) {
        throw new RuntimeException(e);
      }
    }
    return hash;
  }
}