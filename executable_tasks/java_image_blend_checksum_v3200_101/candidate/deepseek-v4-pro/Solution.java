public class Solution {
    private static final long INIT_HASH = 1469598103934665603L;
    private static final long MULT = 1099511628211L;

    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        int pixels = width * height;
        long result = 0;
        for (int iter = 0; iter < iters; iter++) {
            long hash = INIT_HASH;
            for (int i = 0; i < pixels; i++) {
                int base = i << 2;
                int a = src[base + 3] & 0xFF;
                // r
                int blend = blendByte(src[base] & 0xFF, dst[base] & 0xFF, a);
                hash ^= (blend & 0xFFL);
                hash *= MULT;
                // g
                blend = blendByte(src[base + 1] & 0xFF, dst[base + 1] & 0xFF, a);
                hash ^= (blend & 0xFFL);
                hash *= MULT;
                // b
                blend = blendByte(src[base + 2] & 0xFF, dst[base + 2] & 0xFF, a);
                hash ^= (blend & 0xFFL);
                hash *= MULT;
                // alpha = 255
                hash ^= 255L;
                hash *= MULT;
            }
            result = hash;
        }
        return result;
    }

    private static int blendByte(int s, int d, int a) {
        int inv_a = 255 - a;
        return (s * a + d * inv_a + 127) / 255;
    }
}