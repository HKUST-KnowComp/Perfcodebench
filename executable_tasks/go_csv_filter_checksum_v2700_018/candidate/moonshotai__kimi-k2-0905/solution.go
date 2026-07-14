package main

import (
	"strconv"
)

func run(rows []string) uint64 {
	const (
		prime64 = 1099511628211
		offset  = 1469598103934665603
	)
	var h uint64 = offset

	// pre-cast capacity for atoi helpers
	var b, c int

	for _, row := range rows {
		b = 0
		c = 0
		data := []byte(row)
		i := 0
		col := 0

		// parse column 0 -> skip
		for i < len(data) && data[i] != ',' {
			i++
		}
		i++ // skip comma
		col++

		// parse column 1 -> skip
		for i < len(data) && data[i] != ',' {
			i++
		}
		i++
		col++

		// parse column 2 (b)
		for i < len(data) && data[i] != ',' {
			b = b*10 + int(data[i]-'0')
			i++
		}
		i++
		col++

		// parse column 3 (c)
		for i < len(data) && data[i] != ',' {
			c = c*10 + int(data[i]-'0')
			i++
		}
		i++
		col++

		// parse column 4 (hot flag)
		hot := false
		if i+3 <= len(data) && data[i] == 'h' && data[i+1] == 'o' && data[i+2] == 't' {
			hot = true
		}

		v := uint64(b + c + 25)
		if hot {
			v += 29
		}

		h ^= v + 1
		h *= prime64
	}
	return h
}