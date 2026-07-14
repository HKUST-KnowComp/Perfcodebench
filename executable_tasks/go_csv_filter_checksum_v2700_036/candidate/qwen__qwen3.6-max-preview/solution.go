package main

import (
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		pos := 0

		// Skip field 0
		idx := strings.IndexByte(row[pos:], ',')
		if idx < 0 {
			continue
		}
		pos += idx + 1

		// Skip field 1
		idx = strings.IndexByte(row[pos:], ',')
		if idx < 0 {
			continue
		}
		pos += idx + 1

		// Field 2 (b)
		start := pos
		idx = strings.IndexByte(row[pos:], ',')
		if idx < 0 {
			continue
		}
		pos += idx + 1
		b := fastAtoi(row[start : pos-1])

		// Field 3 (c)
		start = pos
		idx = strings.IndexByte(row[pos:], ',')
		if idx < 0 {
			continue
		}
		pos += idx + 1
		c := fastAtoi(row[start : pos-1])

		// Field 4 (hot check)
		hot := len(row) >= pos+3 && row[pos] == 'h' && row[pos+1] == 'o' && row[pos+2] == 't' && (len(row) == pos+3 || row[pos+3] == ',')

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func fastAtoi(s string) int {
	if len(s) == 0 {
		return 0
	}
	neg := false
	i := 0
	if s[0] == '-' {
		neg = true
		i = 1
	} else if s[0] == '+' {
		i = 1
	}
	var n int
	for ; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}
