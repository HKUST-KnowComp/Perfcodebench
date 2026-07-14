import java.util.*;

public class Solution {
    public static long run(List<String> rows) {
        long hash = 1469598103934665603L;
        for (int i = 0; i < rows.size(); i++) {
            String row = rows.get(i);
            int len = row.length();
            
            // We need parts[2], parts[3], and parts[4]
            // parts[0], parts[1], parts[2], parts[3], parts[4]
            // Indices of commas: 
            // 1st comma separates 0 and 1
            // 2nd comma separates 1 and 2
            // 3rd comma separates 2 and 3
            // 4th comma separates 3 and 4
            
            int commaCount = 0;
            int start2 = -1;
            int end2 = -1;
            int start3 = -1;
            int end3 = -1;
            int start4 = -1;
            int end4 = -1;

            for (int j = 0; j < len; j++) {
                if (row.charAt(j) == ',') {
                    commaCount++;
                    if (commaCount == 2) start2 = j + 1;
                    else if (commaCount == 3) {
                        end2 = j;
                        start3 = j + 1;
                    } else if (commaCount == 4) {
                        end3 = j;
                        start4 = j + 1;
                    }
                }
            }
            
            // If the row doesn't have enough commas, we handle it gracefully
            // though the problem implies valid CSV structure.
            if (start2 == -1 || start3 == -1 || start4 == -1) continue;

            // Find end of part 4 (end of string)
            end4 = len;

            // Parse b (parts[2])
            int b = 0;
            for (int k = start2; k < end2; k++) {
                b = b * 10 + (row.charAt(k) - '0');
            }

            // Parse c (parts[3])
            int c = 0;
            for (int k = start3; k < end3; k++) {
                c = c * 10 + (row.charAt(k) - '0');
            }

            // Check hot (parts[4])
            boolean hot = false;
            // parts[4] is "hot"
            if (end4 - start4 == 3) {
                if (row.charAt(start4) == 'h' && 
                    row.charAt(start4 + 1) == 'o' && 
                    row.charAt(start4 + 2) == 't') {
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