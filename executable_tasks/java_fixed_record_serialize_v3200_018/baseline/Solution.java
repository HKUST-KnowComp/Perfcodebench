import java.util.*;

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
    ArrayList<Byte> out = new ArrayList<>();
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      out.clear();
      for (int i = 0; i < ids.length; i++) {
        out.add((byte) ids[i]);
        out.add((byte) (ids[i] >>> 8));
        out.add((byte) (ids[i] >>> 16));
        out.add((byte) (ids[i] >>> 24));
        out.add((byte) ts[i]);
        out.add((byte) (ts[i] >>> 8));
        out.add((byte) (ts[i] >>> 16));
        out.add((byte) (ts[i] >>> 24));
        out.add((byte) levels[i]);
        out.add((byte) (levels[i] >>> 8));
        out.add((byte) lens[i]);
        out.add((byte) (lens[i] >>> 8));
      }
      byte[] bytes = new byte[out.size()];
      for (int i = 0; i < out.size(); i++) bytes[i] = out.get(i);
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}
