package main

import "strings"

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
		flagAdd   = 17
		userMask  = 1023
	)
	hash := uint64(fnvOffset)
	userPrefix := `"user":`
	scorePrefix := `"score":`
	flagStr := `"flag":true`

	for _, row := range rows {
		var user, score int
		flag := strings.Contains(row, flagStr)

		// Parse all user field values to match original behavior
		pos := 0
		for {
			idx := strings.Index(row[pos:], userPrefix)
			if idx == -1 {
				break
			}
			// Locate full part boundaries to match original split logic
			startPrefix := pos + idx
			partStart := 0
			for i := startPrefix; i >= 0; i-- {
				if row[i] == ',' {
					partStart = i + 1
					break
				}
			}
			partEnd := len(row)
			for i := startPrefix; i < len(row); i++ {
				if row[i] == ',' {
					partEnd = i
					break
				}
			}
			// Collect all digits in the part, same as original
			for i := partStart; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					user = user*10 + int(c-'0')
				}
			}
			pos = startPrefix + len(userPrefix)
		}

		// Parse all score field values to match original behavior
		pos = 0
		for {
			idx := strings.Index(row[pos:], scorePrefix)
			if idx == -1 {
				break
			}
			startPrefix := pos + idx
			partStart := 0
			for i := startPrefix; i >= 0; i-- {
				if row[i] == ',' {
					partStart = i + 1
					break
				}
			}
			partEnd := len(row)
			for i := startPrefix; i < len(row); i++ {
				if row[i] == ',' {
					partEnd = i
					break
				}
			}
			for i := partStart; i < partEnd; i++ {
				c := row[i]
				if c >= '0' && c <= '9' {
					score = score*10 + int(c-'0')
				}
			}
			pos = startPrefix + len(scorePrefix)
		}

		// Exact same hash calculation as baseline to preserve checksum correctness
		v := uint64((user & userMask) + score)
		if flag {
			v += flagAdd
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}