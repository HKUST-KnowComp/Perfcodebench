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
                        pos5 = i + 1;
                        break;
                    }
                }
            }
            
            // If row doesn't have enough commas, handle gracefully (though task implies valid CSV)
            if (pos2 == -1 || pos3 == -1 || pos4 == -1) continue;

            // Find end of field 3 (pos3 to pos4)
            int end3 = pos3;
            while (end3 < len && row.charAt(end3) != ',') end3++;
            
            // Find end of field 4 (pos4 to pos5 or end of string)
            int end4 = pos4;
            while (end4 < len && row.charAt(end4) != ',') end4++;

            // Manual integer parsing for field 2 (index 2)
            int b = 0;
            for (int i = pos2; i < pos3; i++) {
                b = b * 10 + (row.charAt(i) - '0');
            }

            // Manual integer parsing for field 3 (index 3)
            int c = 0;
            for (int i = pos3; i < end3; i++) {
                c = c * 10 + (row.charAt(i) - '0');
            }

            // Check for "hot" in field 4 (index 4)
            boolean hot = false;
            if (end4 - pos4 == 3) {
                if (row.charAt(pos4) == 'h' && row.charAt(pos4 + 1) == 'o' && row.charAt(pos4 + 2) == 't') {
                    hot = true;
                }
            }

            long v = (b > 300 ? (b + c) : 0) + (hot ? 29 : 0);
            hash ^= (v + 1);
            hash *= 1099511628211L;
        }
        return hash;
    }
}