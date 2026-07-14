package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual split/parse to avoid allocations from strings.Split
		// Expected format: field0,field1,field2,field3,field4,... where field4 is 'hot' or not
		
		// Find index of 2nd comma (start of field 2)
		idx1 := -1
		idx2 := -1
		idx3 := -1
		idx4 := -1

		start := 0
		commaCount := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					idx2 = i + 1
				}
				if commaCount == 3 {
					idx3 = i + 1
				}
				if commaCount == 4 {
					idx4 = i + 1
					break
				}
			}
		}

		// If row doesn't have enough commas, we skip or handle error
		// But based on baseline, we assume valid structure
		if idx2 == -1 || idx3 == -1 || idx4 == -1 {
			continue
		}

		// Parse field 2 (b)
		b := 0
		for i := idx2; i < len(row) && row[i] != ','; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse field 3 (c)
		c := 0
		// Find end of field 3
		end3 := idx3
		for end3 < len(row) && row[end3] != ',' {
			end3++
		}
		for i := idx3; i < end3; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check field 4 (hot)
		hot := false
		// Field 4 starts at idx4 and ends at next comma or end of string
		// The baseline uses parts[4] == "hot"
		// We check if the substring starting at idx4 is exactly "hot"
		if idx4+3 <= len(row) && row[idx4] == 'h' && row[idx4+1] == 'o' && row[idx4+2] == 't' {
			// Ensure it's not 'hotter' or something by checking next char
			if idx4+3 == len(row) || row[idx4+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 16)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}