import java.util.*;

public class Solution {
  public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
    int n = ids.length;
    int size = n * 12;
    byte[] buf = new byte[size];
    long prime = 1099511628211L;
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      hash = 1469598103934665603L;
      int idx = 0;
      for (int i = 0; i < n; i++) {
        int id = ids[i];
        int t = ts[i];
        short l = levels[i];
        short len = lens[i];
        
        byte b;
        
        b = (byte) id; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (id >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (id >>> 16); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (id >>> 24); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        
        b = (byte) t; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (t >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (t >>> 16); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (t >>> 24); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        
        b = (byte) l; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (l >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        
        b = (byte) len; buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
        b = (byte) (len >>> 8); buf[idx++] = b; hash ^= (b & 0xFFL); hash *= prime;
      }
    }
    return hash;
  }
}