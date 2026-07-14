package main

import (
	"strings"
)

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

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		_, row, _ = strings.Cut(row, ",")
		_, row, _ = strings.Cut(row, ",")
		bStr, row, _ := strings.Cut(row, ",")
		cStr, row, _ := strings.Cut(row, ",")
		hotStr, _, _ := strings.Cut(row, ",")

		b := fastAtoi(bStr)
		c := fastAtoi(cStr)
		hot := hotStr == "hot"

		v := uint64(b + c + 23)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
