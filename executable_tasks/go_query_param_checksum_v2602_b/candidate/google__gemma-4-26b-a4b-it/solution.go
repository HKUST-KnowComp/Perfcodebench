package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var u, score, flag uint64
		
		// Manual single-pass scan to avoid Split and TrimPrefix allocations
		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == '&' {
				// Process segment [start, i)
				// We expect segments to be "u=val", "score=val", "flag=val"
				// Find the '=' position
				eqIdx := -1
				for j := start; j < i; j++ {
					if row[j] == '=' {
						eqIdx = j
						break
					}
				}

				if eqIdx != -1 {
					// Fast integer parsing
					var val uint64
					for k := eqIdx + 1; k < i; k++ {
						char := row[k]
						if char >= '0' && char <= '9' {
							val = val*10 + uint64(char-'0')
						} else {
							break
						}
					}

					// Identify key by first character to avoid string comparison
					keyChar := row[start]
					if keyChar == 'u' {
						u = val
					} else if keyChar == 's' {
						score = val
					} else if keyChar == 'f' {
						flag = val
					}
				}
				start = i + 1
			}
		}

		 v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}