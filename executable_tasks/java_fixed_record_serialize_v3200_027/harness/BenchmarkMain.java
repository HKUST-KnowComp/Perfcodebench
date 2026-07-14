import java.util.*;

public class BenchmarkMain {
  static long checksumBytes(byte[] data) {
    long hash = 1469598103934665603L;
    for (byte b : data) {
      hash ^= (b & 0xFFL);
      hash *= 1099511628211L;
    }
    return hash;
  }

  static void putU32(byte[] dst, int ptr, int v) {
    dst[ptr] = (byte) v;
    dst[ptr + 1] = (byte) (v >>> 8);
    dst[ptr + 2] = (byte) (v >>> 16);
    dst[ptr + 3] = (byte) (v >>> 24);
  }

  static void putU16(byte[] dst, int ptr, int v) {
    dst[ptr] = (byte) v;
    dst[ptr + 1] = (byte) (v >>> 8);
  }

  static Object[] makeInput(int n) {
    int[] ids = new int[n];
    int[] ts = new int[n];
    short[] levels = new short[n];
    short[] lens = new short[n];
    long state = 0x13579BDF2468ACE0L;
    for (int i = 0; i < n; i++) {
      state = state * 6364136223846793005L + 1442695040888963407L;
      ids[i] = (int) state;
      state = state * 2862933555777941757L + 3037000493L;
      ts[i] = (int) state;
      levels[i] = (short) ((state >>> 16) & 7);
      lens[i] = (short) (24 + ((state >>> 24) & 255));
    }
    return new Object[] {ids, ts, levels, lens};
  }

  static long expectedChecksum(int[] ids, int[] ts, short[] levels, short[] lens) {
    byte[] out = new byte[ids.length * 12];
    int ptr = 0;
    for (int i = 0; i < ids.length; i++) {
      putU32(out, ptr, ids[i]); ptr += 4;
      putU32(out, ptr, ts[i]); ptr += 4;
      putU16(out, ptr, levels[i] & 0xFFFF); ptr += 2;
      putU16(out, ptr, lens[i] & 0xFFFF); ptr += 2;
    }
    return checksumBytes(out);
  }

  public static void main(String[] args) {
    Object[] input = makeInput(102200);
    int[] ids = (int[]) input[0];
    int[] ts = (int[]) input[1];
    short[] levels = (short[]) input[2];
    short[] lens = (short[]) input[3];
    long expected = expectedChecksum(ids, ts, levels, lens);
    long start = System.nanoTime();
    long actual = Solution.run(ids, ts, levels, lens, 29);
    long elapsed = System.nanoTime() - start;
    System.out.println("{\"ok\":" + (actual == expected) + ",\"elapsed_ns\":" + elapsed +
        ",\"expected_checksum\":" + expected + ",\"actual_checksum\":" + actual + "}");
    if (actual != expected) throw new RuntimeException("checksum mismatch");
  }
}
