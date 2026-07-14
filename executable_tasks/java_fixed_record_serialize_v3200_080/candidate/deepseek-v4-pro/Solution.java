import java.util.*;

public class Solution {
    private static final long M = 1099511628211L;
    private static final long INIT_HASH = 1469598103934665603L;

    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        long hash = INIT_HASH;
        int len = ids.length;
        for (int i = 0; i < len; i++) {
            int id = ids[i];
            hash ^= (id & 0xFFL);
            hash *= M;
            hash ^= ((id >>> 8) & 0xFFL);
            hash *= M;
            hash ^= ((id >>> 16) & 0xFFL);
            hash *= M;
            hash ^= ((id >>> 24) & 0xFFL);
            hash *= M;

            int time = ts[i];
            hash ^= (time & 0xFFL);
            hash *= M;
            hash ^= ((time >>> 8) & 0xFFL);
            hash *= M;
            hash ^= ((time >>> 16) & 0xFFL);
            hash *= M;
            hash ^= ((time >>> 24) & 0xFFL);
            hash *= M;

            short lev = levels[i];
            hash ^= (lev & 0xFFL);
            hash *= M;
            hash ^= ((lev >>> 8) & 0xFFL);
            hash *= M;

            short lenVal = lens[i];
            hash ^= (lenVal & 0xFFL);
            hash *= M;
            hash ^= ((lenVal >>> 8) & 0xFFL);
            hash *= M;
        }
        return hash;
    }
}