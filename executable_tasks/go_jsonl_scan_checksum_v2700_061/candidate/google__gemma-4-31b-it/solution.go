package main

import (
	"bytes"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	
	// Pre-define byte slices for faster comparison
	userKey := []byte(`"user":`)
	scoreKey := []byte(`"score":`)
	flagTrue := []byte(`"flag":true`)

	for _, row := range rows {
		rowBytes := []byte(row)
		user := 0
		score := 0
		flag := false

		pos := 0
		for pos < len(rowBytes) {
			// Find the next comma or end of string to isolate the part
			end := bytes.IndexByte(rowBytes[pos:], ',')
			if end == -1 {
				end = len(rowBytes) - pos
			}
			part := rowBytes[pos : pos+end]

			if bytes.Contains(part, userKey) {
				for _, c := range part {
					if c >= '0' && c <= '9' {
						user = user*10 + int(c-'0')
					}
				}
			} else if bytes.Contains(part, scoreKey) {
				for _, c := range part {
					if c >= '0' && c <= '9' {
						score = score*10 + int(c-'0')
					}
				}
			} else if bytes.Contains(part, flagTrue) {
				flag = true
			}

			pos += end + 1
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}