package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		comma := 0
		idx := 0
		for idx < len(row) {
			if row[idx] == ',' {
				comma++
				if comma == 2 {
					idx++
					break
				}
			}
			idx++
		}

		b := 0
		neg := false
		if idx < len(row) && row[idx] == '-' {
			neg = true
			idx++
		}
		for idx < len(row) && row[idx] != ',' {
			b = b*10 + int(row[idx]-'0')
			idx++
		}
		if neg {
			b = -b
		}
		if idx < len(row) {
			idx++
		}

		c := 0
		neg := false
		if idx < len(row) && row[idx] == '-' {
			neg = true
			idx++
		}
		for idx < len(row) && row[idx] != ',' {
			c = c*10 + int(row[idx]-'0')
			idx++
		}
		if neg {
			c = -c
		}
		if idx < len(row) {
			idx++
		}

		hot := false
		if idx+3 <= len(row) && row[idx] == 'h' && row[idx+1] == 'o' && row[idx+2] == 't' {
			if idx+3 == len(row) || row[idx+3] == ',' {
				hot = true
			}
		}

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}