java
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;

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
    int totalBytes = ids.length * 12;
    ByteArrayOutputStream bos = new ByteArrayOutputStream(totalBytes);
    DataOutputStream dos = new DataOutputStream(bos);
    long hash = 0;
    try {
      for (int iter = 0; iter < iters; iter++) {
        bos.reset();
        for (int i = 0; i < ids.length; i++) {
          dos.writeInt(ids[i]);
          dos.writeInt(ts[i]);
          dos.writeShort(levels[i]);
          dos.writeShort(lens[i]);
        }
        hash = checksumBytes(bos.toByteArray());
      }
    } catch (IOException e) {
      throw new RuntimeException(e);
    }
    return hash;
  }
}