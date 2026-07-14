package main

import (
	"bytes"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const multiplier uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		data := []byte(row)
		pos := 0
		for pos < len(data) {
			// Find the start of a key
			start := bytes.IndexByte(data[pos:], '"')
			if start == -1 {
				break
			}
			pos += start

			// Check for keys
			if bytes.HasPrefix(data[pos:], "\"user\":") {
				pos += 8
				for pos < len(data) && (data[pos] >= '0' && data[pos] <= '9') {
					user = user*10 + int(data[pos]-'0')
					pos++
				}
			} else if bytes.HasPrefix(data[pos:], "\"score\":") {
				pos += 9
				for pos < len(data) && (data[pos] >= '0' && data[pos] <= '9') {
					score = score*10 + int(data[pos]-'0')
					pos++
				}
			} else if bytes.HasPrefix(data[pos:], "\"flag\":true") {
				flag = true
				pos += 13
			} else {
				pos++
			}
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= multiplier
	}
	return hash
}