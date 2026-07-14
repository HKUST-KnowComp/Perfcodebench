package main

import (
	"strings"
)

func parseUint(s string) int {
	var n int
	for _, c := range s {
		n = n*10 + int(c-'0')
	}
	return n
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		firstAmp := strings.Index(row, "&")
		u := parseUint(row[2:firstAmp])
		remaining := row[firstAmp+1:]
		secondAmp := strings.Index(remaining, "&")
		score := parseUint(remaining[6:secondAmp])
		flag := parseUint(remaining[secondAmp+1+5:])
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}