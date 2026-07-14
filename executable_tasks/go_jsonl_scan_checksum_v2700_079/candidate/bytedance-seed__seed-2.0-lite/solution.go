package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const userPrefix = `"user":`
	const scorePrefix = `"score":`
	const flagStr = `"flag":true`
	const fnvPrime = 1099511628211

	for _, row := range rows {
		var user, score int
		var flag bool

		if pos := strings.Index(row, userPrefix); pos != -1 {
			for i := pos + len(userPrefix); i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				user = user*10 + int(c-'0')
			}
		}

		if pos := strings.Index(row, scorePrefix); pos != -1 {
			for i := pos + len(scorePrefix); i < len(row); i++ {
				c := row[i]
				if c < '0' || c > '9' {
					break
				}
				score = score*10 + int(c-'0')
			}
		}

		flag = strings.Contains(row, flagStr)

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}