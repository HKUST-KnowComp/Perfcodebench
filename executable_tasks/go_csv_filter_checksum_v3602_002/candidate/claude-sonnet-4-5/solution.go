package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual field extraction without allocation
		comma1 := 0
		for i := 0; i < len(row); i++ {
			if row[i] == ',' {
				comma1 = i
				break
			}
		}
		comma2 := comma1 + 1
		for i := comma2; i < len(row); i++ {
			if row[i] == ',' {
				comma2 = i
				break
			}
		}
		comma3 := comma2 + 1
		for i := comma3; i < len(row); i++ {
			if row[i] == ',' {
				comma3 = i
				break
			}
		}
		comma4 := comma3 + 1
		for i := comma4; i < len(row); i++ {
			if row[i] == ',' {
				comma4 = i
				break
			}
		}
		
		// Parse field 2 (b) manually
		b := 0
		neg := false
		start := comma2 + 1
		if start < comma3 && row[start] == '-' {
			neg = true
			start++
		}
		for i := start; i < comma3; i++ {
			b = b*10 + int(row[i]-'0')
		}
		if neg {
			b = -b
		}
		
		// Parse field 3 (c) manually
		c := 0
		neg = false
		start = comma3 + 1
		if start < comma4 && row[start] == '-' {
			neg = true
			start++
		}
		for i := start; i < comma4; i++ {
			c = c*10 + int(row[i]-'0')
		}
		if neg {
			c = -c
		}
		
		// Check field 4 inline
		hot := comma4+1 < len(row) && row[comma4+1] == 'h'
		
		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
