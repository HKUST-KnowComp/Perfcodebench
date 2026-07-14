package main

import "strings"

func parseNum(s string, start int) int {
	v := 0
	for start < len(s) {
		c := s[start]
		if c < '0' || c > '9' {
			break
		}
		v = v*10 + int(c-'0')
		start++
	}
	return v
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		uPos := strings.Index(row, "u=")
		sPos := strings.Index(row, "&score=")
		fPos := strings.Index(row, "&flag=")
		u := parseNum(row, uPos+2)
		score := parseNum(row, sPos+7)
		flag := parseNum(row, fPos+6)
		v := uint64((u & 2047) + score + flag*19)
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
