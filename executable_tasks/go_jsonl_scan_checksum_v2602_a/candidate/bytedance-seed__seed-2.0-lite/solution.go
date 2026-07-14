package main

import "strings"

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
		keyUser   = `"user":`
		keyScore  = `"score":`
		keyFlag   = `"flag":true`
		keyLen    = 6 // Length of user/score keys, both 6 bytes
	)
	hash := uint64(fnvOffset)
	for _, row := range rows {
		var user, score int
		flag := false

		// Extract user ID directly from row without splitting
		if idx := strings.Index(row, keyUser); idx != -1 {
			for i := idx + keyLen; i < len(row); i++ {
				b := row[i]
				if b < '0' || b > '9' {
					break
				}
				user = user*10 + int(b-'0')
			}
		}

		// Extract score directly from row
		if idx := strings.Index(row, keyScore); idx != -1 {
			for i := idx + keyLen; i < len(row); i++ {
				b := row[i]
				if b < '0' || b > '9' {
					break
				}
				score = score*10 + int(b-'0')
			}
		}

		// Check for flag presence
		if strings.Contains(row, keyFlag) {
			flag = true
		}

		// Exact same checksum update logic as original to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}