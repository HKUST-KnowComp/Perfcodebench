import java.util.*;

public class Solution {
    public static long run(int[] ids, int[] ts, short[] levels, short[] lens, int iters) {
        if (iters <= 0) {
            return 0L;
        }
        long hash = 1469598103934665603L;
        long mult = 1099511628211L;
        for (int i = 0; i < ids.length; i++) {
            int id = ids[i];
            hash = (hash ^ (id & 0xFFL)) * mult;
            hash = (hash ^ ((id >>> 8) & 0xFFL)) * mult;
            hash = (hash ^ ((id >>> 16) & 0xFFL)) * mult;
            hash = (hash ^ ((id >>> 24) & 0xFFL)) * mult;

            int time = ts[i];
            hash = (hash ^ (time & 0xFFL)) * mult;
            hash = (hash ^ ((time >>> 8) & 0xFFL)) * mult;
            hash = (hash ^ ((time >>> 16) & 0xFFL)) * mult;
            hash = (hash ^ ((time >>> 24) & 0xFFL)) * mult;

            short lvl = levels[i];
            hash = (hash ^ (lvl & 0xFFL)) * mult;
            hash = (hash ^ ((lvl >>> 8) & 0xFFL)) * mult;

            short len = lens[i];
            hash = (hash ^ (len & 0xFFL)) * mult;
            hash = (hash ^ ((len >>> 8) & 0xFFL)) * mult;
        }
        return hash;
    }
}