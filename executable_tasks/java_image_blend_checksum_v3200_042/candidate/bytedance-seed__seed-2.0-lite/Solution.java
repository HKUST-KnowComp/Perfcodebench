public class Solution {
    private static int blendByte(int s, int d, int a) {
        int numerator = s * a + d * (255 - a) + 127;
        return (int) ((numerator * 0x8081L) >> 31);
    }

    private static long checksumBytes(byte[] data) {
        long hash = 1469598103934665603L;
        for (byte b : data) {
            hash ^= (b & 0xFFL);
            hash *= 1099511628211L;
        }
        return hash;
    }

    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        byte[] out = new byte[src.length];
        long hash = 0;
        int pixels = width * height;
        for (int iter = 0; iter < iters; iter++) {
            int offset = 0;
            for (int i = 0; i < pixels; i++) {
                int a = src[offset + 3] & 0xFF;
                out[offset] = (byte) blendByte(src[offset] & 0xFF, dst[offset] & 0xFF, a);
                out[offset + 1] = (byte) blendByte(src[offset + 1] & 0xFF, dst[offset + 1] & 0xFF, a);
                out[offset + 2] = (byte) blendByte(src[offset + 2] & 0xFF, dst[offset + 2] & 0xFF, a);
                out[offset + 3] = (byte) 255;
                offset += 4;
            }
            hash = checksumBytes(out);
        }
        return hash;
    }
}