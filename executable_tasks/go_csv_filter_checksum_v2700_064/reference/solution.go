package main

import "strings"

func parseAt(s string, start int) int {
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
		p1 := strings.IndexByte(row, ',')
		p2 := strings.IndexByte(row[p1+1:], ',') + p1 + 1
		p3 := strings.IndexByte(row[p2+1:], ',') + p2 + 1
		p4 := strings.IndexByte(row[p3+1:], ',') + p3 + 1
		b := parseAt(row, p2+1)
		c := parseAt(row, p3+1)
		hot := row[p4+1] == 'h'
		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
