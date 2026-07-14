package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	// fastParseUint parses an unsigned integer from s[start:end].
	// It returns the value and the number of bytes consumed.
	// It deliberately ignores any error (same behaviour as strconv.Atoi).
	fastParseUint := func(s string, start int) uint64 {
		v := uint64(0)
		for i := start; i < len(s); i++ {
			c := s[i]
			if c < '0' || c > '9' {
				break
			}
			v = v*10 + uint64(c-'0')
		}
		return v
	}

	var hash uint64 = 1469598103934665603

	for _, row := range rows {
		// Scan for the three '=' positions directly.
		// We know the exact layout: u=<int>&score=<int>&flag=<int>
		uStart := 2 // skip "u="
		scoreStart := strings.IndexByte(row, '&') + 7 // skip "&score="
		flagStart := strings.IndexByte(row[scoreStart:], '&')
		if flagStart == -1 {
			flagStart = len(row)
		} else {
			flagStart += scoreStart + 1 + 5 // skip "&flag="
		}

		u := fastParseUint(row, uStart)
		score := fastParseUint(row, scoreStart)
		flag := fastParseUint(row, flagStart)

		v := (u & 2047) + score + flag*19
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}