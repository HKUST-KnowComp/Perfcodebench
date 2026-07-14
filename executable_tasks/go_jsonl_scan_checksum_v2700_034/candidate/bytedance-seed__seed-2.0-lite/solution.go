package main

import "strings"

func run(rows []string) uint64 {
	const (
		offset           = 1469598103934665603
		prime            = 1099511628211
		userPrefix       = `"user":`
		scorePrefix      = `"score":`
		flagSubstr       = `"flag":true`
		lenUserPrefix    = len(userPrefix)
		lenScorePrefix   = len(scorePrefix)
	)
	hash := uint64(offset)
	for _, row := range rows {
		var user, score int
		flag := false

		// Parse all user values to match original summation logic
		start := 0
		for {
			idx := strings.Index(row[start:], userPrefix)
			if idx == -1 {
				break
			}
			pos := start + idx + lenUserPrefix
			for ; pos < len(row); pos++ {
				b := row[pos]
				if b < '0' || b > '9' {
					break
				}
				user = user*10 + int(b-'0')
			}
			start = pos
		}

		// Parse all score values to match original summation logic
		start = 0
		for {
			idx := strings.Index(row[start:], scorePrefix)
			if idx == -1 {
				break
			}
			pos := start + idx + lenScorePrefix
			for ; pos < len(row); pos++ {
				b := row[pos]
				if b < '0' || b > '9' {
					break
				}
				score = score*10 + int(b-'0')
			}
			start = pos
		}

		// Check flag condition identical to original
		if strings.Contains(row, flagSubstr) {
			flag = true
		}

		// Exact same hash calculation to preserve correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}