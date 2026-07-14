package main

import (
	"bytes"
)

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const prime uint64 = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		// Convert to bytes for faster indexing and avoid string slicing overhead
		data := []byte(row)
		pos := 0
		for pos < len(data) {
			// Look for the start of a key
			if data[pos] == '"' {
				// Check for "user":
				if pos+6 <= len(data) && data[pos+1] == 'u' && data[pos+2] == 's' && data[pos+3] == 'e' && data[pos+4] == 'r' && data[pos+5] == '"' {
					pos += 6
					// Skip to the value
					for pos < len(data) && (data[pos] == ':' || data[pos] == ' ') {
						pos++
					}
					// Parse integer
					for pos < len(data) && data[pos] >= '0' && data[pos] <= '9' {
						user = user*10 + int(data[pos]-'0')
						pos++
					}
					continue
				}
				// Check for "score":
				if pos+7 <= len(data) && data[pos+1] == 's' && data[pos+2] == 'c' && data[pos+3] == 'o' && data[pos+4] == 'r' && data[pos+5] == 'e' && data[pos+6] == '"' {
					pos += 7
					for pos < len(data) && (data[pos] == ':' || data[pos] == ' ') {
						pos++
					}
					for pos < len(data) && data[pos] >= '0' && data[pos] <= '9' {
						score = score*10 + int(data[pos]-'0')
						pos++
					}
					continue
				}
				// Check for "flag":
				if pos+6 <= len(data) && data[pos+1] == 'f' && data[pos+2] == 'l' && data[pos+3] == 'a' && data[pos+4] == 'g' && data[pos+5] == '"' {
					pos += 6
					for pos < len(data) && (data[pos] == ':' || data[pos] == ' ') {
						pos++
					}
					if pos+4 <= len(data) && data[pos] == 't' && data[pos+1] == 'r' && data[pos+2] == 'u' && data[pos+3] == 'e' {
						flag = true
						pos += 4
					}
					continue
				}
			}
			pos++
		}

		v := uint64((user & 1023) + score)
		if flag {
			v += 17
		}
		hash ^= v + 1
		hash *= prime
	}
	return hash
}
