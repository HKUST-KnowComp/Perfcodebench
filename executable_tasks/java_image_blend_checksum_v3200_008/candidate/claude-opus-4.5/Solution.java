import java.util.*;

public class Solution {
  private static final byte[][] BLEND_TABLE = new byte[256][256];
  
  static {
    for (int a = 0; a < 256; a++) {
      int invA = 255 - a;
      for (int diff = 0; diff < 256; diff++) {
        BLEND_TABLE[a][diff] = (byte)((diff * a + 127) / 255);
      }
    }
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int len = pixels * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i << 2;
        int a = src[base + 3] & 0xFF;
        int invA = 255 - a;
        byte[] blendA = BLEND_TABLE[a];
        byte[] blendInvA = BLEND_TABLE[invA];
        
        int sr = src[base] & 0xFF;
        int sg = src[base + 1] & 0xFF;
        int sb = src[base + 2] & 0xFF;
        int dr = dst[base] & 0xFF;
        int dg = dst[base + 1] & 0xFF;
        int db = dst[base + 2] & 0xFF;
        
        out[base] = (byte)((blendA[sr] & 0xFF) + (blendInvA[dr] & 0xFF));
        out[base + 1] = (byte)((blendA[sg] & 0xFF) + (blendInvA[dg] & 0xFF));
        out[base + 2] = (byte)((blendA[sb] & 0xFF) + (blendInvA[db] & 0xFF));
        out[base + 3] = (byte)255;
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