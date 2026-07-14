package main

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	for _, row := range rows {
		var u, score, flag int
		i := 2 // Skip leading "u="
		// Parse user ID u
		for ; i < len(row); i++ {
			c := row[i]
			if c == '&' {
				break
			}
			u = u*10 + int(c-'0')
		}
		// Skip past "&score=" (1 byte for & + 6 bytes for "score=" = 7 bytes)
		i += 7
		// Parse score
		for ; i < len(row); i++ {
			c := row[i]
			if c == '&' {
				break
			}
			score = score*10 + int(c-'0')
		}
		// Skip past "&flag=" (1 byte for & + 5 bytes for "flag=" = 6 bytes)
		i += 6
		// Parse flag
		for ; i < len(row); i++ {
			c := row[i]
			if c == '&' {
				break
			}
			flag = flag*10 + int(c-'0')
		}
		// Preserve original hash logic for identical checksum output
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}