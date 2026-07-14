package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		user := 0
		score := 0
		flag := strings.Contains(row, "\"flag\":true")

		// Parse user ID from the last occurrence of "user" key, matching original behavior
		if userIdx := strings.LastIndex(row, "\"user\":"); userIdx != -1 {
			// Find end of the current comma-separated part
			partEnd := strings.Index(row[userIdx:], ",")
			if partEnd == -1 {
				partEnd = len(row)
			} else {
				partEnd += userIdx
			}
			// Collect all digits in this part, same as original implementation
			for i := userIdx; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				}
			}
		}

		// Parse score from the last occurrence of "score" key
		if scoreIdx := strings.LastIndex(row, "\"score\":"); scoreIdx != -1 {
			partEnd := strings.Index(row[scoreIdx:], ",")
			if partEnd == -1 {
				partEnd = len(row)
			} else {
				partEnd += scoreIdx
			}
			for i := scoreIdx; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				}
			}
		}

		// Same checksum calculation logic as original to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}