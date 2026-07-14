public class Solution {
  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    int pixels = width * height;
    int totalBytes = pixels * 4;
    byte[] out = new byte[totalBytes];
    
    for (int i = 0; i < pixels; i++) {
      int base = i * 4;
      int a = src[base + 3] & 0xFF;
      int invA = 255 - a;
      out[base] = (byte) (( (src[base] & 0xFF) * a + (dst[base] & 0xFF) * invA + 127) / 255);
      out[base+1] = (byte) (( (src[base+1] & 0xFF) * a + (dst[base+1] & 0xFF) * invA + 127) / 255);
      out[base+2] = (byte) (( (src[base+2] & 0xFF) * a + (dst[base+2] & 0xFF) * invA + 127) / 255);
      out[base+3] = (byte) 255;
    }
    
    long hash = 1469598103934665603L;
    for (int i = 0; i < totalBytes; i++) {
      hash ^= (out[i] & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }
}