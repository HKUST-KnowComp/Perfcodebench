package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
		
		commaCount := 0
		var idx2, idx3, idx4 int
		var start2, end2, start3, end3, start4, end4 int
		
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					start2 = i + 1
				}
				if commaCount == 3 {
					end2 = i
					start3 = i + 1
				}
				if commaCount == 4 {
					end3 = i
					start4 = i + 1
				}
				if commaCount == 5 {
					end4 = i
					break
				}
			}
		}
		
		// If row doesn't have enough commas, handle gracefully (though task implies valid CSV)
		if commaCount < 4 {
			continue
		}
		if end4 == 0 {
			end4 = len(row)
		}

		// Fast integer parsing for parts[2]
		b := 0
		for i := start2; i < end2; i++ {
			b = b*10 + int(row[i]-'0')
		}

		// Fast integer parsing for parts[3]
		c := 0
		for i := start3; i < end3; i++ {
			c = c*10 + int(row[i]-'0')
		}

		// Check hot status in parts[4]
		// parts[4] is row[start4:end4]
		hot := false
		if end4-start4 == 3 {
			if row[start4] == 'h' && row[start4+1] == 'o' && row[start4+2] == 't' {
				hot = true
			}
		}

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}