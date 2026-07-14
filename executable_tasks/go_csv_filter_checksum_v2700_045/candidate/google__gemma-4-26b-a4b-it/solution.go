package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Manual scan to avoid strings.Split allocations
		// parts[2] is the 3rd field, parts[3] is the 4th, parts[4] is the 5th
		
		fieldIdx := 0
		start := 0
		var b, c int
		var hot bool
		
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				if fieldIdx == 2 {
					// Parse b
					val := 0
					for j := start; j < i; j++ {
						if row[j] >= '0' && row[j] <= '9' {
							val = val*10 + int(row[j]-'0')
						}
						b = val
					}
					} else if fieldIdx == 3 {
					// Parse c
					val := 0
					for j := start; j < i; j++ {
						if row[j] >= '0' && row[j] <= '9' {
							val = val*10 + int(row[j]-'0')
						}
						}
					c = val
					} else if fieldIdx == 4 {
					// Check hot
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
				}

				fieldIdx++
				start = i + 1
			}
		}

		// The logic requires parts[2], parts[3], and parts[4]
		// If the row is malformed, the variables remain 0/false
		v := uint64(b + c + 29)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}