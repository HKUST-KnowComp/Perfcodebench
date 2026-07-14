package main

import "strings"

func parseIntAt(s string, start int) int {
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
		userPos := strings.Index(row, "\"user\":")
		scorePos := strings.Index(row, "\"score\":")
		flag := strings.Index(row, "\"flag\":true") >= 0
		user := parseIntAt(row, userPos+len("\"user\":"))
		score := parseIntAt(row, scorePos+len("\"score\":"))
		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
