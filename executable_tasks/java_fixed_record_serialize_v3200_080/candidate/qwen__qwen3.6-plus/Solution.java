public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int size = n * 12;
    byte[] buf = new byte[size];
    long hash = 0;
    for (int iter = 0; iter < iters; iter++) {
      int idx = 0;
      hash = 1469598103934665603L;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        byte b = (byte) id; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (id >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (id >>> 16); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (id >>> 24); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        
        int t = ts[i];
        b = (byte) t; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (t >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (t >>> 16); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (t >>> 24); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        
        short l = levels[i];
        b = (byte) l; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (l >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        
        short len = lens[i];
        b = (byte) len; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
        b = (byte) (len >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= 1099511628211L;
      }
    }
    return hash;
  }
}