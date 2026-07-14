package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		idx1 := strings.Index(row, ",")
		idx2 := strings.Index(row[idx1+1:], ",") + idx1 + 1
		idx3 := strings.Index(row[idx2+1:], ",") + idx2 + 1
		idx4 := strings.Index(row[idx3+1:], ",") + idx3 + 1

		part2 := row[idx2+1 : idx3]
		part3 := row[idx3+1 : idx4]
		part4 := row[idx4+1:]

		b, _ := strconv.Atoi(part2)
		c, _ := strconv.Atoi(part3)
		hot := part4 == "hot"

		v := uint64(b + c + 20)
		if hot {
			v += 29
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}