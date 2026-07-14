package main

import "strings"

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const fnvPrime = 1099511628211
	for _, row := range rows {
		var user, score int
		var flag bool
		b := []byte(row)

		// Extract last occurrence of user field to match baseline behavior
		if userIdx := strings.LastIndex(row, `"user":`); userIdx != -1 {
			u := 0
			i := userIdx + 7 // Length of `"user":`
			for i < len(b) && b[i] >= '0' && b[i] <= '9' {
				u = u*10 + int(b[i]-'0')
				i++
			}
			user = u
		}

		// Extract last occurrence of score field to match baseline behavior
		if scoreIdx := strings.LastIndex(row, `"score":`); scoreIdx != -1 {
			s := 0
			i := scoreIdx + 7 // Length of `"score":`
			for i < len(b) && b[i] >= '0' && b[i] <= '9' {
				s = s*10 + int(b[i]-'0')
				i++
			}
			score = s
		}

		// Check for flag true anywhere in row, matches baseline
		flag = strings.Contains(row, `"flag":true`)

		// Exact same hash calculation as baseline to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}