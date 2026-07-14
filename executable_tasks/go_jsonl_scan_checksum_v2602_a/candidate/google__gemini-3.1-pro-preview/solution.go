package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const (
		fnvPrime = 1099511628211
		userKey  = "\"user\":"
		scoreKey = "\"score\":"
		flagKey  = "\"flag\":true"
	)

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		start := 0
		rowLen := len(row)
		for i := 0; i <= rowLen; i++ {
			if i == rowLen || row[i] == ',' {
				part := row[start:i]
				
				// Check for keys in the current comma-separated segment
				// The baseline logic sums all digits in a part if it contains the key.
				if strings.Contains(part, userKey) {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
						}
					}
				} else if strings.Contains(part, scoreKey) {
					for j := 0; j < len(part); j++ {
						c := part[j]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
						}
					}
				} else if strings.Contains(part, flagKey) {
					flag = true
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