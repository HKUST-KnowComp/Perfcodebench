package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	const (
		prime = 1099511628211
		offset = 1469598103934665603
	)
	var h uint64 = offset
	for _, row := range rows {
		b := row
		// skip first field
		i := 0
		for i < len(b) && b[i] != ',' {
			i++
		}
		i++ // comma
		// skip second field
		for i < len(b) && b[i] != ',' {
			i++
		}
		i++ // comma
		// parse third field (int)
		v1 := 0
		for i < len(b) && b[i] != ',' {
			v1 = v1*10 + int(b[i]-'0')
			i++
		}
		i++ // comma
		// parse fourth field (int)
		v2 := 0
		for i < len(b) && b[i] != ',' {
			v2 = v2*10 + int(b[i]-'0')
			i++
		}
		i++ // comma
		// check fifth field == "hot"
		hot := i+3 <= len(b) && b[i] == 'h' && b[i+1] == 'o' && b[i+2] == 't'
		v := uint64(v1 + v2 + 15)
		if hot {
			v += 29
		}
		h ^= v + 1
		h *= prime
	}
	return h
}