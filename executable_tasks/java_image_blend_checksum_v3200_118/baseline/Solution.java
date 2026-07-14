import java.util.*;

public class Solution {
  static int blendByte(int s, int d, int a) {
    return (s * a + d * (255 - a) + 127) / 255;
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
    ArrayList<Byte> out = new ArrayList<>(src.length);
    long hash = 0;
    int pixels = width * height;
    for (int iter = 0; iter < iters; iter++) {
      out.clear();
      for (int i = 0; i < pixels; i++) {
        int base = i * 4;
        int a = src[base + 3] & 0xFF;
        out.add((byte) blendByte(src[base] & 0xFF, dst[base] & 0xFF, a));
        out.add((byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a));
        out.add((byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a));
        out.add((byte) 255);
      }
      byte[] bytes = new byte[out.size()];
      for (int i = 0; i < out.size(); i++) bytes[i] = out.get(i);
      hash = checksumBytes(bytes);
    }
    return hash;
  }
}
