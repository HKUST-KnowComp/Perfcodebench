package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		// Find comma positions for fields 2, 3, 4 (0-indexed)
		// Format: field0,field1,field2,field3,field4,...
		var commaIdx [5]int
		commaCount := 0
		for i := 0; i < len(row) && commaCount < 5; i++ {
			if row[i] == ',' {
				commaIdx[commaCount] = i
				commaCount++
			}
		}

		// Parse field 2 (between comma 1 and comma 2)
		b := 0
		for i := commaIdx[1] + 1; i < commaIdx[2]; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Parse field 3 (between comma 2 and comma 3)
		c := 0
		for i := commaIdx[2] + 1; i < commaIdx[3]; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check field 4 for "hot" (between comma 3 and comma 4 or end)
		// "hot" is 3 chars, check if it matches
		start4 := commaIdx[3] + 1
		hot := false
		if commaCount > 4 {
			if commaIdx[4]-start4 == 3 &&
				row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't' {
				hot = true
			}
		} else {
			// field 4 goes to end of string
			if len(row)-start4 == 3 &&
				row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't' {
				hot = true
			}
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
