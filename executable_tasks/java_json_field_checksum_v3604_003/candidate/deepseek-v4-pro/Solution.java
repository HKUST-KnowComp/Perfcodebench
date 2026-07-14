import java.util.List;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        final long multiplier = 1099511628211L;
        for (String row : rows) {
            char[] chars = row.toCharArray();
            int pos = 5; // skip past {\"u\":
            // parse u
            long u = 0;
            while (chars[pos] != ',') {
                u = u * 10 + (chars[pos] - '0');
                pos++;
            }
            // skip ,\"s\":
            pos++; // skip comma
            pos += 4; // skip \"s\":
            // parse s
            long s = 0;
            while (chars[pos] != ',') {
                s = s * 10 + (chars[pos] - '0');
                pos++;
            }
            // skip ,\"f\":
            pos++; // skip comma
            pos += 4; // skip \"f\":
            // parse f boolean
            boolean f = (chars[pos] == 't');
            // skip to next comma
            while (chars[pos] != ',') {
                pos++;
            }
            // compute contribution
            long v = (u & 1023) + s + (f ? 17 : 0);
            hash ^= (v + 1);
            hash *= multiplier;
        }
        return hash;
    }
}