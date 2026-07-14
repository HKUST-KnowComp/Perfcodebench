package main

import (
	"strings"
)

func parseInt(s string) int {
	var n int
	for _, c := range s {
		n = n*10 + int(c-'0')
	}
	return n
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	fnvPrime := uint64(1099511628211)
	for _, row := range rows {
		firstAmp := strings.Index(row, "&")
		remAfterFirst := row[firstAmp:]
		secondAmpRel := strings.Index(remAfterFirst, "&")
		secondAmp := firstAmp + secondAmpRel

		u := parseInt(row[2:firstAmp])
		score := parseInt(row[firstAmp+7:secondAmp])
		flag := parseInt(row[secondAmp+6:])

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}