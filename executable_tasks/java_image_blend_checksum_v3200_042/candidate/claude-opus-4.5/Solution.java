import java.util.*;

public class Solution {
  private static final byte[][] BLEND_TABLE = new byte[256][256];
  
  static {
    for (int a = 0; a < 256; a++) {
      int invA = 255 - a;
      for (int s = 0; s < 256; s++) {
        int result = 0;
        for (int d = 0; d < 256; d++) {
          result = (s * a + d * invA + 127) / 255;
        }
        BLEND_TABLE[a][s] = (byte) result;
      }
    }
    for (int a = 0; a < 256; a++) {
      int invA = 255 - a;
      for (int d = 0; d < 256; d++) {
        for (int s = 0; s < 256; s++) {
          BLEND_TABLE[a][s] = (byte) ((s * a + d * invA + 127) / 255);
        }
      }
    }
  }
  
  private static final int[][] BLEND_LUT = new int[256][256 * 256];
  
  static {
    for (int a = 0; a < 256; a++) {
      int invA = 255 - a;
      int[] lut = BLEND_LUT[a];
      for (int s = 0; s < 256; s++) {
        int sTimesA = s * a + 127;
        for (int d = 0; d < 256; d++) {
          lut[(s << 8) | d] = (sTimesA + d * invA) / 255;
        }
      }
    }
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0, base = 0; i < pixels; i++, base += 4) {
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        
        int s0 = src[base] & 0xFF;
        int d0 = dst[base] & 0xFF;
        out[base] = (byte) ((s0 * a + d0 * invA + 127) / 255);
        
        int s1 = src[base + 1] & 0xFF;
        int d1 = dst[base + 1] & 0xFF;
        out[base + 1] = (byte) ((s1 * a + d1 * invA + 127) / 255);
        
        int s2 = src[base + 2] & 0xFF;
        int d2 = dst[base + 2] & 0xFF;
        out[base + 2] = (byte) ((s2 * a + d2 * invA + 127) / 255);
        
        out[base + 3] = (byte) 255;
      }
      
      hash = 1469598103934665603L;
      for (int i = 0; i < len; i++) {
        hash ^= (out[i] & 0xFFL);
        hash *= 1099511628211L;
      }
    }
    return hash;
  }
}