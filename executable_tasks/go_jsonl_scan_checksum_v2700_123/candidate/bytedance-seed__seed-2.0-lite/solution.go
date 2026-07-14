package main

import "strings"

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const userKey = `"user":`
	const scoreKey = `"score":`
	const flagStr = `"flag":true`
	keyLen := len(userKey)

	for _, row := range rows {
		var user, score int
		var flag bool

		// Parse user ID directly from raw row
		if idx := strings.Index(row, userKey); idx != -1 {
			for i := idx + keyLen; i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				user = user*10 + int(c-'0')
			}
		}

		// Parse score directly from raw row
		if idx := strings.Index(row, scoreKey); idx != -1 {
			for i := idx + keyLen; i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				score = score*10 + int(c-'0')
			}
		}

		// Single check for flag across full row
		if strings.Contains(row, flagStr) {
			flag = true
		}

		// Exact original hash computation preserved for correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}