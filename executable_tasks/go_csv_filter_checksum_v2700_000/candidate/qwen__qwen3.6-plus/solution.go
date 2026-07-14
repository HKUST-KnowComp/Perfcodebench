package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		idx := 0
		// Skip first two fields
		for i := 0; i < 2; i++ {
			for idx < len(row) && row[idx] != ',' {
				idx++
			}
			if idx < len(row) {
				idx++
			}
		}

		// Parse field 2 (b)
		b := 0
		if idx < len(row) && row[idx] == '-' {
			idx++
			for idx < len(row) && row[idx] != ',' {
				b = b*10 - int(row[idx]-'0')
				idx++
			}
		} else {
			for idx < len(row) && row[idx] != ',' {
				b = b*10 + int(row[idx]-'0')
				idx++
			}
		}
		if idx < len(row) {
			idx++
		}

		// Parse field 3 (c)
		c := 0
		if idx < len(row) && row[idx] == '-' {
			idx++
			for idx < len(row) && row[idx] != ',' {
				c = c*10 - int(row[idx]-'0')
				idx++
			}
		} else {
			for idx < len(row) && row[idx] != ',' {
				c = c*10 + int(row[idx]-'0')
				idx++
			}
		}
		if idx < len(row) {
			idx++
		}

		// Check field 4 (hot)
		hot := false
		if idx+3 <= len(row) && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't' {
			if idx+3 == len(row) || row[idx+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 7)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}