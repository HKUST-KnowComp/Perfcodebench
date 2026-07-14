package main

import "strings"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		i := strings.IndexByte(row, ',')
		if i < 0 {
			continue
		}
		row = row[i+1:]

		i = strings.IndexByte(row, ',')
		if i < 0 {
			continue
		}
		row = row[i+1:]

		i = strings.IndexByte(row, ',')
		if i < 0 {
			continue
		}
		b := fastAtoi(row[:i])
		row = row[i+1:]

		i = strings.IndexByte(row, ',')
		if i < 0 {
			continue
		}
		c := fastAtoi(row[:i])
		row = row[i+1:]

		i = strings.IndexByte(row, ',')
		var hot bool
		if i < 0 {
			hot = row == "hot"
		} else {
			hot = row[:i] == "hot"
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func fastAtoi(s string) int {
	n := 0
	neg := false
	if len(s) > 0 && s[0] == '-' {
		neg = true
		s = s[1:]
	}
	for i := 0; i < len(s); i++ {
		n = n*10 + int(s[i]-'0')
	}
	if neg {
		return -n
	}
	return n
}
