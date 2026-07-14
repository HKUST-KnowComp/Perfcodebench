package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		parts := csvSplit(row)
		b := atoi(parts[2])
		c := atoi(parts[3])
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

func csvSplit(line string) []string {
	parts := make([]string, 0, 5)
	last := 0
	for i := 0; i < len(line); i++ {
		if line[i] == ',' {
			parts = append(parts, line[last:i])
			last = i + 1
		}
	}
	parts = append(parts, line[last:])
	return parts
}

func atoi(s string) int {
	n := 0
	neg := false
	for i := 0; i < len(s); i++ {
		if s[i] >= '0' && s[i] <= '9' {
			n = n*10 + int(s[i]-'0')
		} else if s[i] == '-' && i == 0 {
			neg = true
		}
	}
	if neg {
		n = -n
	}
	return n
}