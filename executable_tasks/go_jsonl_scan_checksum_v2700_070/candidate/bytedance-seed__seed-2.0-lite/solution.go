package main

import "strings"

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
		userKey   = `"user":`
		scoreKey  = `"score":`
		flagKey   = `"flag":true`
	)
	hash := uint64(fnvOffset)
	for _, row := range rows {
		var user, score int
		flag := strings.Contains(row, flagKey)

		// Parse final user ID from the row to match original overwrite behavior
		if idx := strings.LastIndex(row, userKey); idx != -1 {
			pos := idx + len(userKey)
			for ; pos < len(row); pos++ {
				c := row[pos]
				if c < '0' || c > '9' {
					break
				}
				user = user*10 + int(c-'0')
			}
		}

		// Parse final score from the row
		if idx := strings.LastIndex(row, scoreKey); idx != -1 {
			pos := idx + len(scoreKey)
			for ; pos < len(row); pos++ {
				c := row[pos]
				if c < '0' || c > '9' {
					break
				}
				score = score*10 + int(c-'0')
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