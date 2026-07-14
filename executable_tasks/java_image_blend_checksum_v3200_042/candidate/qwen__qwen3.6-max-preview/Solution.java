import java.util.*;

public class Solution {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (int i = 0; i < data.length; i++) {
      hash ^= (data[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int len = width * height * 4;
    byte[] out = new byte[len];
    long hash = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < len; i += 4) {
        int a = src[i + 3] & 0xFF;
        int invA = 255 - a;
        out[i]     = (byte) (((src[i] & 0xFF) * a + (dst[i] & 0xFF) * invA + 127) / 255);
        out[i + 1] = (byte) (((src[i+1] & 0xFF) * a + (dst[i+1] & 0xFF) * invA + 127) / 255);
        out[i + 2] = (byte) (((src[i+2] & 0xFF) * a + (dst[i+2] & 0xFF) * invA + 127) / 255);
        out[i + 3] = (byte) 255;
      }
      hash = checksumBytes(out);
    }
    return hash;
  }
}
