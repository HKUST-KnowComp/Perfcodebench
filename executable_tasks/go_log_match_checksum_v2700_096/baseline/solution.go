package main

import "regexp"

var pat = regexp.MustCompile(`^[A-Z]{2}[0-9]{4}-[0-9A-F]{8}$`)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		v := uint64(0)
		if pat.MatchString(row) {
			v = uint64(row[2] + row[7])
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}
