package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103946265135 // Corrected FNV offset basis for 64-bit if needed, but keeping original logic
	hash = 1469598103934665603

	for _, row := range rows {
		// Manual split to avoid allocations
		// parts[2] is index 2, parts[3] is index 3, parts[4] is index 4
		// We need to find the 2nd, 3rd, and 4th commas
		
		commaCount := 0
		startIdx := 0
		var p2Start, p2End, p3Start, p3End, p4Start, p4End int

		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				commaCount++
				if commaCount == 2 {
					p2Start = i + 1
				}
				if commaCount == 3 {
					p2End = i
					p3Start = i + 1
				}
				if commaCount == 4 {
					p3End = i
					p4Start = i + 1
					// We don't necessarily need p4End if we check the rest of the string
					break
				}
			}
		}

		// Parse b (parts[2])
		b := 0
		for i := p2Start; i < p2End; i++ {
			if row[i] >= '0' && row[i] <= '9' {
				b = b*10 + int(row[i]-'0')
			}
		}

		// Parse c (parts[3])
		c := 0
		for i := p3Start; i < p3End; i++ {
			if row[i] >= '0' && row[i] <= '9' {
				c = c*10 + int(row[i]-'0')
			}
		}

		// Check hot (parts[4])
		hot := false
		// parts[4] starts at p4Start and goes to end of row
		// Check if it is exactly "hot"
		if len(row)-p4Start == 3 && row[p4Start] == 'h' && row[p4Start+1] == 'o' && row[p4Start+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 15)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}