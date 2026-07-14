package main

import "strings"

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const userPrefix = `"user":`
	const scorePrefix = `"score":`
	const flagTrueStr = `"flag":true`
	for _, row := range rows {
		var user, score int
		var flag bool

		// Parse user ID
		if idx := strings.Index(row, userPrefix); idx != -1 {
			for i := idx + len(userPrefix); i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				user = user*10 + int(c-'0')
			}
		}

		// Parse score
		if idx := strings.Index(row, scorePrefix); idx != -1 {
			for i := idx + len(scorePrefix); i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				score = score*10 + int(c-'0')
			}
		}

		// Check flag status
		flag = strings.Contains(row, flagTrueStr)

		// Maintain exact hash calculation from baseline to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}