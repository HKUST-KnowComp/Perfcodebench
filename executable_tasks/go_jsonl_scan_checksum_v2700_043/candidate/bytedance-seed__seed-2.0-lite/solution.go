package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 14695981039346656037
	for _, row := range rows {
		user := 0
		score := 0
		flag := strings.Contains(row, `"flag":true`)

		// Extract last matching user field, matching original's overwrite behavior
		if idx := strings.LastIndex(row, `"user":`); idx != -1 {
			// Find start of the comma-separated part containing user field
			partStart := 0
			for i := idx - 1; i >= 0; i-- {
				if row[i] == ',' {
					partStart = i + 1
					break
				}
			}
			// Find end of the comma-separated part
			partEnd := len(row)
			for i := idx; i < len(row); i++ {
				if row[i] == ',' {
					partEnd = i
					break
				}
			}
			// Parse all digits in the part, identical to original logic
			for i := partStart; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				}
			}
		}

		// Extract last matching score field
		if idx := strings.LastIndex(row, `"score":`); idx != -1 {
			partStart := 0
			for i := idx - 1; i >= 0; i-- {
				if row[i] == ',' {
					partStart = i + 1
					break
				}
			}
			partEnd := len(row)
			for i := idx; i < len(row); i++ {
				if row[i] == ',' {
					partEnd = i
					break
				}
			}
			for i := partStart; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				}
			}
		}

		// Exact same checksum calculation as original
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}