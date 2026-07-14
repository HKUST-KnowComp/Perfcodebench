package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var user, score int
		var flag bool

		// Extract user ID directly from full row, no split overhead
		if userIdx := strings.Index(row, `"user":`); userIdx != -1 {
			i := userIdx + 6 // Advance past `"user":`
			for ; i < len(row) && row[i] >= '0' && row[i] <= '9'; i++ {
				user = user*10 + int(row[i]-'0')
			}
		}

		// Extract score directly from full row
		if scoreIdx := strings.Index(row, `"score":`); scoreIdx != -1 {
			i := scoreIdx + 7 // Advance past `"score":`
			for ; i < len(row) && row[i] >= '0' && row[i] <= '9'; i++ {
				score = score*10 + int(row[i]-'0')
			}
		}

		// Check for flag directly in full row
		flag = strings.Contains(row, `"flag":true`)

		// Exact same hash computation as original to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}