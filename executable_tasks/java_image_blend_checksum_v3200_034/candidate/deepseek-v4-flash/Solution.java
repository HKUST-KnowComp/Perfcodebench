import java.util.*;

public class Solution {
    public static long run(byte[] src, byte[] dst, int width, int height, int iters) {
        if (iters <= 0) {
            return 0;
        }
        int pixels = width * height;
        int total = pixels * 4;
        byte[] blend = new byte[total];
        // blend once
        for (int i = 0; i < pixels; i++) {
            int base = i * 4;
            int a = src[base + 3] & 0xFF;
            int invA = 255 - a;
            int s0 = src[base] & 0xFF;
            int d0 = dst[base] & 0xFF;
            blend[base] = (byte) ((s0 * a + d0 * invA + 127) / 255);
            int s1 = src[base + 1] & 0xFF;
            int d1 = dst[base + 1] & 0xFF;
            blend[base + 1] = (byte) ((s1 * a + d1 * invA + 127) / 255);
            int s2 = src[base + 2] & 0xFF;
            int d2 = dst[base + 2] & 0xFF;
            blend[base + 2] = (byte) ((s2 * a + d2 * invA + 127) / 255);
            blend[base + 3] = (byte) 255;
        }
        long hash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long h = 1469598103934665603L;
            for (int i = 0; i < total; i++) {
                h ^= (blend[i] & 0xFFL);
                h *= 1099511628211L;
            }
            hash = h;
        }
        return hash;
    }
}