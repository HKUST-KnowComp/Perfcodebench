import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    int x = s * a + d * (255 - a) + 127;
    return (x * 0x8081) >>> 23;
  }

  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
    if (src.length == 0) return 0;
    byte[] outArray = new byte[src.length];
    long hash = 0;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        outArray[base] = (byte) blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
        outArray[base + 1] = (byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
        outArray[base + 2] = (byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
        outArray[base + 3] = (byte) 255;
      }
      hash = checksumBytes(outArray);
    }
    return hash;
  }
}