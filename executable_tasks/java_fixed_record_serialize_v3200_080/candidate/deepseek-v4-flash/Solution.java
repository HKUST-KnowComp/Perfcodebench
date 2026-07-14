import java.util.*;

public class Solution {
    private static final long FNV_OFFSET_BASIS = 1469598103934665603L;
    private static final long FNV_PRIME = 1099511628211L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        final int n = ids.length;
        final byte[] buf = new byte[n * 12];
        int pos = 0;
        for (int i = 0; i < n; i++) {
            int id = ids[i];
            buf[pos++] = (byte) (id);
            buf[pos++] = (byte) (id >>>  8);
            buf[pos++] = (byte) (id >>> 16);
            buf[pos++] = (byte) (id >>> 24);
            int t = ts[i];
            buf[pos++] = (byte) (t);
            buf[pos++] = (byte) (t >>>  8);
            buf[pos++] = (byte) (t >>> 16);
            buf[pos++] = (byte) (t >>> 24);
            int lv = levels[i] & 0xFFFF;
            buf[pos++] = (byte) (lv);
            buf[pos++] = (byte) (lv >>>  8);
            int ln = lens[i] & 0xFFFF;
            buf[pos++] = (byte) (ln);
            buf[pos++] = (byte) (ln >>>  8);
        }

        long hash = 0;
        for (int iter = 0; iter < iters; iter++) {
            long h = FNV_OFFSET_BASIS;
            for (byte b : buf) {
                h ^= (b & 0xFFL);
                h *= FNV_PRIME;
            }
            hash = h;
        }
        return hash;
    }
}