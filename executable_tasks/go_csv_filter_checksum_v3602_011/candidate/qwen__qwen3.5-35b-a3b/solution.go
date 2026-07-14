package main

import (
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := strings.SplitN(row, ",", 5)
		b := parseInt(parts[2])
		c := parseInt(parts[3])
		hot := parts[4] == "hot"
		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}

func parseInt(s string) int {
	var n int
	var sign int = 1
	var i int = 0
	if len(s) > 0 && s[0] == '-' {
		sign = -1
		i = 1
	}
	for i < len(s) {
		n = n*10 + int(s[i] - '0')
		i++
	}
	return n * sign
}