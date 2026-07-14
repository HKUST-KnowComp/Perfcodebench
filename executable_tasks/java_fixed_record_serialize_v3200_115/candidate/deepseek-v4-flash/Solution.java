import java.util.*;

public class Solution {
  private static final long FNV_OFFSET = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int totalBytes = n * 12;
    byte[] buffer = new byte[totalBytes];
    long hash = 0;

    for (int iter = 0; iter < iters; iter++) {
      long h = FNV_OFFSET;
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buffer[idx] = (byte) id;
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (id >>> 8);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (id >>> 16);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (id >>> 24);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;

        int t = ts[i];
        buffer[idx] = (byte) t;
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (t >>> 8);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (t >>> 16);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (t >>> 24);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;

        short lvl = levels[i];
        buffer[idx] = (byte) lvl;
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (lvl >>> 8);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;

        short len = lens[i];
        buffer[idx] = (byte) len;
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
        buffer[idx] = (byte) (len >>> 8);
        h ^= (buffer[idx] & 0xFFL);
        h *= FNV_PRIME;
        idx++;
      }
      hash = h;
    }
    return hash;
  }
}