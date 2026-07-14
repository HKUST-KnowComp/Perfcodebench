package main

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)

	for _, row := range rows {
		var user int
		var score int
		var flag bool

		start := 0
		for i := 0; i <= len(row); i++ {
			if i == len(row) || row[i] == ',' {
				segment := row[start:i]
				
				// Check for keys within the segment
				hasUser := false
				hasScore := false
				
				// Manual implementation of strings.Contains and digit extraction
				// to match baseline behavior exactly.
				for j := 0; j < len(segment); j++ {
					if !hasUser && j+6 < len(segment) && segment[j] == '"' && segment[j+1] == 'u' && segment[j+2] == 's' && segment[j+3] == 'e' && segment[j+4] == 'r' && segment[j+5] == '"' && segment[j+6] == ':' {
						hasUser = true
					}
					if !hasScore && j+7 < len(segment) && segment[j] == '"' && segment[j+1] == 's' && segment[j+2] == 'c' && segment[j+3] == 'o' && segment[j+4] == 'r' && segment[j+5] == 'e' && segment[j+6] == '"' && segment[j+7] == ':' {
						hasScore = true
					}
					if !flag && j+10 < len(segment) && segment[j] == '"' && segment[j+1] == 'f' && segment[j+2] == 'l' && segment[j+3] == 'a' && segment[j+4] == 'g' && segment[j+5] == '"' && segment[j+6] == ':' && segment[j+7] == 't' && segment[j+8] == 'r' && segment[j+9] == 'u' && segment[j+10] == 'e' {
						flag = true
					}
				}

				if hasUser {
					for j := 0; j < len(segment); j++ {
						c := segment[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						}
					}
				} else if hasScore {
					for j := 0; j < len(segment); j++ {
						c := segment[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						}
					}
				}

				start = i + 1
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}