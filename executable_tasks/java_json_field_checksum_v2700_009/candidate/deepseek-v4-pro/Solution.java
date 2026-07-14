import java.util.*;

public class Solution {
    private static final long INITIAL_HASH = 1469598103934665603L;
    private static final long MULTIPLIER = 1099511628211L;

    public static long run(List<String> rows) {
        long hash = INITIAL_HASH;
        for (String row : rows) {
            // Locate u value
            int uIdx = row.indexOf("\"u\":");
            if (uIdx < 0) continue; // defensive, should not happen
            int uStart = uIdx + 5;
            int uEnd = row.indexOf(',', uStart);
            int u = 0;
            for (int i = uStart; i < uEnd; i++) {
                u = u * 10 + (row.charAt(i) - '0');
            }

            // Locate s value
            int sIdx = row.indexOf("\"s\":", uEnd);
            if (sIdx < 0) continue;
            int sStart = sIdx + 5;
            int sEnd = row.indexOf(',', sStart);
            int s = 0;
            for (int i = sStart; i < sEnd; i++) {
                s = s * 10 + (row.charAt(i) - '0');
            }

            // Locate f value
            int fIdx = row.indexOf("\"f\":", sEnd);
            if (fIdx < 0) continue;
            int fStart = fIdx + 5;
            boolean f = row.charAt(fStart) == 't'; // 'true' or 'false'

            // Compute checksum contribution
            long v = (u & 1023L) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= MULTIPLIER;
        }
        return hash;
    }
}