package main

import "strings"

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	const userPrefix = `"user":`
	const scorePrefix = `"score":`
	const flagSubstr = `"flag":true`
	prefixLen := len(userPrefix)

	for _, row := range rows {
		var user, score int
		flag := strings.Contains(row, flagSubstr)

		// Parse all user values to exactly match original logic
		offset := 0
		rowLen := len(row)
		for {
			idx := strings.Index(row[offset:], userPrefix)
			if idx == -1 {
				break
			}
			start := offset + idx + prefixLen
			for start < rowLen && row[start] >= '0' && row[start] <= '9' {
				user = user*10 + int(row[start]-'0')
				start++
			}
			offset = start
		}

		// Parse all score values to exactly match original logic
		offset = 0
		for {
			idx := strings.Index(row[offset:], scorePrefix)
			if idx == -1 {
				break
			}
			start := offset + idx + prefixLen
			for start < rowLen && row[start] >= '0' && row[start] <= '9' {
				score = score*10 + int(row[start]-'0')
				start++
			}
			offset = start
		}

		// Preserve original hash calculation for full correctness
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}