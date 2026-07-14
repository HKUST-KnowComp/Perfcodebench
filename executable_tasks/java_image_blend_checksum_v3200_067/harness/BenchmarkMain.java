import java.util.*;

public class BenchmarkMain {
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

  static byte[][] makeInput(int pixels) {
    byte[] src = new byte[pixels * 4];
    byte[] dst = new byte[pixels * 4];
    long state = 0x123456789ABCDEF0L;
    for (int i = 0; i < pixels; i++) {
      int base = i * 4;
      for (int c = 0; c < 3; c++) {
        state = state * 6364136223846793005L + 1442695040888963407L;
        src[base + c] = (byte) (state >>> 56);
        state = state * 2862933555777941757L + 3037000493L;
        dst[base + c] = (byte) (state >>> 56);
      }
      state = state * 6364136223846793005L + 1442695040888963407L;
      src[base + 3] = (byte) (state >>> 56);
      dst[base + 3] = (byte) 255;
    }
    return new byte[][] {src, dst};
  }

  static long expectedChecksum(byte[] src, byte[] dst, int width, int height) {
    byte[] out = new byte[src.length];
    for (int i = 0; i < width * height; i++) {
      int base = i * 4;
      int a = src[base + 3] & 0xFF;
      out[base] = (byte) blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
      out[base + 1] = (byte) blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
      out[base + 2] = (byte) blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
      out[base + 3] = (byte) 255;
    }
    return checksumBytes(out);
  }

  public static void main(String[] args) {
    int width = 352;
    int height = 200;
    int iters = 26;
    byte[][] input = makeInput(width * height);
    byte[] src = input[0];
    byte[] dst = input[1];
    long expected = expectedChecksum(src, dst, width, height);
    long start = System.nanoTime();
    long actual = Solution.run(src, dst, width, height, iters);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
