package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const fnvPrime = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Instead of strings.Split, we iterate through the row manually or use Index to find segments.
		// The baseline splits by "," and checks each part for specific keys.
		start := 0
		for start < len(row) {
			end := strings.IndexByte(row[start:], ',')
			var part string
			if end == -1 {
				part = row[start:]
				start = len(row)
			} else {
				part = row[start : start+end]
				start += end + 1
			}

			// Check for keys within the segment
			if strings.Contains(part, "\"user\":") {
				for i := 0; i < len(part); i++ {
					c := part[i]
					if c >= '0' && c <= '9' {
						user = user*10 + int(c-'0')
					}
				}
			} else if strings.Contains(part, "\"score\":") {
				for i := 0; i < len(part); i++ {
					c := part[i]
					if c >= '0' && c <= '9' {
						score = score*10 + int(c-'0')
					}
				}
			} else if strings.Contains(part, "\"flag\":true") {
				flag = true
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