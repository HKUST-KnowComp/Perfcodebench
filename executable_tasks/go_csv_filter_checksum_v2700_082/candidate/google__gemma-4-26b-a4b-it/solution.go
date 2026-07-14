package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var b, c int
		var hot bool
		
		// Manual single-pass scan to avoid strings.Split allocations
		// parts[2] is index 2, parts[3] is index 3, parts[4] is index 4
		
		col := 0
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				if col == 2 {
					// Parse b
					for j := start; j < i; j++ {
						b = b*10 + int(row[j]-'0')
					}
				} else if col == 3 {
					// Parse c
					for j := start; j < i; j++ {
						c = c*10 + int(row[j]-'0')
					}
				} else if col == 4 {
					// Check hot
					if i-start == 3 && row[start] == 'h' && row[start+1] == 'o' && row[start+2] == 't' {
						hot = true
					}
				}
				col++
				start = i + 1
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