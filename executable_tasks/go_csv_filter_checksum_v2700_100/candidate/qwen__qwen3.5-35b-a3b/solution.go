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
		v := uint64(b + c + 15)
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
	var i int
	if s[0] == '-' {
		i = 1
	} else {
		i = 0
	}
	for ; i < len(s); i++ {
		n = n*10 + int(s[i] - '0')
	}
	if s[0] == '-' {
		n = -n
	}
	return n
}