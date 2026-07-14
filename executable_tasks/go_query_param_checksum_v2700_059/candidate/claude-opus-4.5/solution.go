package main

import (
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// row format: "u=<int>&score=<int>&flag=<int>"
		// Find first '&'
		i1 := strings.IndexByte(row, '&')
		// u value starts at index 2 (after "u=")
		u := parseIntFast(row, 2, i1)

		// Find second '&'
		i2 := strings.IndexByte(row[i1+1:], '&') + i1 + 1
		// score value starts at i1+1+6 = i1+7 (after "&score=")
		score := parseIntFast(row, i1+7, i2)

		// flag value starts at i2+1+5 = i2+6 (after "&flag=")
		flag := parseIntFast(row, i2+6, len(row))

		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

// parseIntFast parses a non-negative integer from s[start:end].
// Assumes valid decimal digits; handles optional leading '-' for negative.
func parseIntFast(s string, start, end int) int {
	if start >= end {
		return 0
	}
	neg := false
	if s[start] == '-' {
		neg = true
		start++
	}
	n := 0
	for i := start; i < end; i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}
