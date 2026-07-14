package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// We need parts[2], parts[3], and parts[4]=="hot"
		// Parse by manual scanning to avoid allocations.
		b := 0
		c := 0
		hot := false

		// Find start of field 3 (index 2)
		i := 0
		commaCount := 0
		for i < len(row) && commaCount < 2 {
			if row[i] == ',' {
				commaCount++
			}
			i++
		}
		// i is now at the byte after the second comma (start of field 3) or len(row)

		// Parse integer at field 3
		sign := 1
		if i < len(row) {
			ch := row[i]
			if ch == '-' {
				sign = -1
				i++
			} else if ch == '+' {
				i++
			}
		}
		val := 0
		for i < len(row) {
			ch := row[i]
			if ch >= '0' && ch <= '9' {
				val = val*10 + int(ch-'0')
				i++
			} else {
				break
			}
		}
		b = sign * val
		// Skip to next comma (end of field 3)
		for i < len(row) && row[i] != ',' {
			i++
		}
		if i < len(row) && row[i] == ',' {
			i++
		}

		// Parse integer at field 4
		sign = 1
		if i < len(row) {
			ch := row[i]
			if ch == '-' {
				sign = -1
				i++
			} else if ch == '+' {
				i++
			}
		}
		val = 0
		for i < len(row) {
			ch := row[i]
			if ch >= '0' && ch <= '9' {
				val = val*10 + int(ch-'0')
				i++
			} else {
				break
			}
		}
		c = sign * val
		// Skip to next comma (end of field 4)
		for i < len(row) && row[i] != ',' {
			i++
		}
		if i < len(row) && row[i] == ',' {
			i++
		}

		// Check if field 5 equals "hot" exactly
		if i+3 <= len(row) {
			if row[i] == 'h' && row[i+1] == 'o' && row[i+2] == 't' {
				j := i + 3
				if j == len(row) || row[j] == ',' {
					hot = true
				}
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
