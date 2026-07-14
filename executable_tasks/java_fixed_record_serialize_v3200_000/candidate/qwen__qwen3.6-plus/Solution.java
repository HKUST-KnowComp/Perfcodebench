import java.util.*;

public class Solution {
  private static final long FNV_INIT = 1469598103934665603L;
  private static final long FNV_PRIME = 1099511628211L;

  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int size = n * 12;
    byte[] buf = new byte[size];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      int pos = 0;
      long h = FNV_INIT;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        buf[pos++] = (byte) id;
        h ^= (id & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (id >>> 8);
        h ^= ((id >>> 8) & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (id >>> 16);
        h ^= ((id >>> 16) & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (id >>> 24);
        h ^= ((id >>> 24) & 0xFFL); h *= FNV_PRIME;

        int t = ts[i];
        buf[pos++] = (byte) t;
        h ^= (t & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (t >>> 8);
        h ^= ((t >>> 8) & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (t >>> 16);
        h ^= ((t >>> 16) & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (t >>> 24);
        h ^= ((t >>> 24) & 0xFFL); h *= FNV_PRIME;

        short l = levels[i];
        buf[pos++] = (byte) l;
        h ^= (l & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (l >>> 8);
        h ^= ((l >>> 8) & 0xFFL); h *= FNV_PRIME;

        short len = lens[i];
        buf[pos++] = (byte) len;
        h ^= (len & 0xFFL); h *= FNV_PRIME;
        buf[pos++] = (byte) (len >>> 8);
        h ^= ((len >>> 8) & 0xFFL); h *= FNV_PRIME;
      }
      hash = h;
    }
    return hash;
  }
}