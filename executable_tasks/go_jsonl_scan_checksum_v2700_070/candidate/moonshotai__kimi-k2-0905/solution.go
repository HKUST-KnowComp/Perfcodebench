package main

import "strings"

// run computes the required checksum over all rows.
// Optimized: single pass over raw string, no allocations, byte-level parsing.
func run(rows []string) uint64 {
	const (
		prime64 = 1099511628211
		offset  = 1469598103934665603
	)

	hash := offset

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Fast scan over the raw bytes of the row.
		// We look for the three fields in one pass.
		for i := 0; i < len(row); i++ {
			// Check for "user":
			if i+6 < len(row) && row[i] == '"' && row[i+1] == 'u' && row[i+2] == 's' && row[i+3] == 'e' && row[i+4] == 'r' && row[i+5] == '"' && row[i+6] == ':' {
				i += 7
				// Skip leading spaces
				for i < len(row) && row[i] == ' ' {
					i++
				}
				// Parse integer
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					user = user*10 + int(row[i]-'0')
					i++
				}
				i-- // compensate for outer loop increment
				continue
			}

			// Check for "score":
			if i+7 < len(row) && row[i] == '"' && row[i+1] == 's' && row[i+2] == 'c' && row[i+3] == 'o' && row[i+4] == 'r' && row[i+5] == 'e' && row[i+6] == '"' && row[i+7] == ':' {
				i += 8
				// Skip leading spaces
				for i < len(row) && row[i] == ' ' {
					i++
				}
				// Parse integer
				for i < len(row) && row[i] >= '0' && row[i] <= '9' {
					score = score*10 + int(row[i]-'0')
					i++
				}
				i-- // compensate
				continue
			}

			// Check for "flag":true
			if i+11 < len(row) && row[i] == '"' && row[i+1] == 'f' && row[i+2] == 'l' && row[i+3] == 'a' && row[i+4] == 'g' && row[i+5] == '"' && row[i+6] == ':' && row[i+7] == 't' && row[i+8] == 'r' && row[i+9] == 'u' && row[i+10] == 'e' {
				flag = true
				i += 10
				continue
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime64
	}
	return hash
}