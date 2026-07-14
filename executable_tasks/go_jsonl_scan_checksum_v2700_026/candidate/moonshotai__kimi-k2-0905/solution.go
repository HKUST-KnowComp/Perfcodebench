package main

import (
	"strconv"
	"strings"
)

func run(rows []string) uint64 {
	const (
		fnvOffset = 1469598103934665603
		fnvPrime  = 1099511628211
	)
	hash := uint64(fnvOffset)

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// single pass over the row
		i := 0
		n := len(row)
		for i < n {
			// find next key start
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			i++ // skip opening quote

			// identify field name
			start := i
			for i < n && row[i] != '"' {
				i++
			}
			if i >= n {
				break
			}
			key := row[start:i]
			i++ // skip closing quote

			// skip colon and any spaces
			for i < n && (row[i] == ':' || row[i] == ' ') {
				i++
			}
			if i >= n {
				break
			}

			switch key {
			case "user":
				// parse integer value
				neg := false
				if row[i] == '-' {
					neg = true
					i++
				}
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				if neg {
					v = -v
				}
				user = v
			case "score":
				// parse integer value
				neg := false
				if row[i] == '-' {
					neg = true
					i++
				}
				v := 0
				for i < n && row[i] >= '0' && row[i] <= '9' {
					v = v*10 + int(row[i]-'0')
					i++
				}
				if neg {
					v = -v
				}
				score = v
			case "flag":
				// check for true
				if i+4 <= n && row[i:i+4] == "true" {
					flag = true
					i += 4
				} else {
					// skip false or any other token
					for i < n && row[i] != ',' && row[i] != '}' {
						i++
					}
				}
			default:
				// skip unknown field value
				for i < n && row[i] != ',' && row[i] != '}' {
					i++
				}
			}

			// skip to next comma or end
			for i < n && row[i] != ',' {
				i++
			}
			if i < n && row[i] == ',' {
				i++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= fnvPrime
	}
	return hash
}