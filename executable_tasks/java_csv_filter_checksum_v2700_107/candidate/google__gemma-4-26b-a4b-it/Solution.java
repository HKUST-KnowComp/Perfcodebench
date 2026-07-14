import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (String row : rows) {
            int len = row.length();
            int commaCount = 0;
            int pos2 = -1;
            int pos3 = -1;
            int pos4 = -1;
            int pos5 = -1;

            // Single pass to find comma positions
            for (int i = 0; i < len; i++) {
                if (row.charAt(i) == ',') {
                    commaCount++;
                    if (commaCount == 2) pos2 = i + 1;
                    else if (commaCount == 3) pos3 = i + 1;
                    else if (commaCount == 4) pos4 = i + 1;
                    else if (commaCount == 5) {
                        pos5 = i;
                        break;
                    }
                }
            }
            
            // If row doesn't have enough commas, handle gracefully (though task implies valid CSV)
            if (pos2 == -1 || pos3 == -1 || pos4 == -1) continue;

            // Manual integer parsing for field 2 (b)
            int b = 0;
            int end2 = (pos3 == -1) ? len : pos3 - 1;
            for (int i = pos2; i < end2; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }

            // Manual integer parsing for field 3 (c)
            int c = 0;
            int end3 = (pos4 == -1) ? len : pos4 - 1;
            for (int i = pos3; i < end3; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }

            // Check for "hot" in field 4
            boolean hot = false;
            if (pos4 != -1) {
                int end4 = (pos5 == -1) ? len : pos5;
                int field4Len = end4 - pos4;
                if (field4Len == 3) {
                    if (row.charAt(pos4) == 'h' && row.charAt(pos4 + 1) == 'o' && row.charAt(pos4 + 2) == 't') {
                        hot = true;
                    }
                }
            }

            long v = (b > 300 ? (long) b + c : 0L) + (hot ? 29L : 0L);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}