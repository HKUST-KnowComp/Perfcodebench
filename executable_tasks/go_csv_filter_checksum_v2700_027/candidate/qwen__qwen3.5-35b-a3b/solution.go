package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		firstComma := strings.Index(row, ",")
		secondComma := strings.Index(row[firstComma+1:], ",") + firstComma +1
		thirdComma := strings.Index(row[secondComma+1:], ",") + secondComma +1
		fourthComma := strings.Index(row[thirdComma+1:], ",") + thirdComma +1

		field2 := row[secondComma+1 : thirdComma]
		field3 := row[thirdComma+1 : fourthComma]

		b, _ := strconv.Atoi(field2)
		c, _ := strconv.Atoi(field3)

		hot := len(row) - fourthComma -1 == 3 && row[fourthComma+1] == 'h' && row[fourthComma+2] == 'o' && row[fourthComma+3] == 't'

		v := uint64(b + c + 11)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}