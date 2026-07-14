package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		s := row
		var commas [5]int
		pos := 0
		for i := 0; i < 5; i++ {
			idx := strings.Index(s[pos:], ",")
			commas[i] = pos + idx
			pos = commas[i] + 1
		}
		field2 := s[commas[1]+1 : commas[2]]
		field3 := s[commas[2]+1 : commas[3]]
		field4 := s[commas[3]+1 : commas[4]]
		b, _ := strconv.Atoi(field2)
		c, _ := strconv.Atoi(field3)
		hot := field4 == "hot"
		v := uint64(b + c + 25)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}