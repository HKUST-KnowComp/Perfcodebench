package main

import (
	"strings"
)

func run(rows []string) uint64 {
	hash := uint64(1469598103934665603)
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		firstAmp := strings.Index(row, "&")
		remaining := row[firstAmp+1:]
		secondAmp := strings.Index(remaining, "&")
		secondAmp += firstAmp + 1

		var u int
		for i := 2; i < firstAmp; i++ {
			u = u*10 + int(row[i]-'0')
		}

		var score int
		scoreStart := firstAmp + 7
		for i := scoreStart; i < secondAmp; i++ {
			score = score*10 + int(row[i]-'0')
		}

		var flag int
		flagStart := secondAmp + 6
		for i := flagStart; i < len(row); i++ {
			flag = flag*10 + int(row[i]-'0')
		}

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}