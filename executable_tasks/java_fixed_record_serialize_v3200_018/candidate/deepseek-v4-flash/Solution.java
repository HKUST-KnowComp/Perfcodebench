import java.util.*;

public class Solution {
    static long checksumBytes(byte[] data) {
        long hash = 1469598103934665603L;
        for (byte b : data) {
            hash ^= (b & 0xFFL);
            hash *= 1099511628211L;
        }
        return hash;
    }

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        final int n = ids.length;
        final int recordSize = 12;
        final byte[] buf = new byte[n * recordSize];
        long hash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long h = 1469598103934665603L;
            int pos = 0;
            for (int i = 0; i < n; i++) {
                int id = ids[i];
                buf[pos] = (byte) id;
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (id >>> 8);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (id >>> 16);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (id >>> 24);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                int t = ts[i];
                buf[pos] = (byte) t;
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (t >>> 8);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (t >>> 16);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (t >>> 24);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                short level = levels[i];
                buf[pos] = (byte) level;
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (level >>> 8);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                short len = lens[i];
                buf[pos] = (byte) len;
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
                buf[pos] = (byte) (len >>> 8);
                h ^= (buf[pos] & 0xFFL);
                h *= 1099511628211L;
                pos++;
            }
            hash = h;
        }
        return hash;
    }
}