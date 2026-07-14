import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int count = ids.length;
    byte[] buffer = new byte[count * 12];
    long hash = 0;
    final long FNV_INIT = 1469598103934665603L;
    final long FNV_PRIME = 1099511628211L;

    for (int iter = 0; iter < iters; iter++) {
      hash = FNV_INIT;
      int pos = 0;
      for (int i = 0; i < count; i++) {
        int id = ids[i];
        int t = ts[i];
        short level = levels[i];
        short len = lens[i];

        // write id (4 bytes LE)
        byte b = (byte) id;
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (id >>> 8);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (id >>> 16);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (id >>> 24);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;

        // write ts (4 bytes LE)
        b = (byte) t;
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (t >>> 8);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (t >>> 16);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (t >>> 24);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;

        // write level (2 bytes LE)
        b = (byte) level;
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (level >>> 8);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;

        // write len (2 bytes LE)
        b = (byte) len;
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
        b = (byte) (len >>> 8);
        buffer[pos++] = b;
        hash ^= (b & 0xFFL);
        hash *= FNV_PRIME;
      }
    }
    return hash;
  }
}
