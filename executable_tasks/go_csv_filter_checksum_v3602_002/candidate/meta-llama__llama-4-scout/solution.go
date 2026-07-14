package main

import (
	"encoding/csv"
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		// Use csv parser for more efficient and correct handling of quoted values and escapes
		parts := csv.NewReader(strings.NewReader(row)).Read()
		if len(parts) < 5 {
			continue // Skip rows with insufficient fields
		}
		b, _ := strconv.Atoi(parts[2])
		c, _ := strconv.Atoi(parts[3])
		v := uint64(b + c + 11)
		if parts[4] == "hot" {
			v += 29
		}
		// Use a non-allocating xor and multiplication to keep performance high
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}