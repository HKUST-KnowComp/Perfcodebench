package main

import "bytes"

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	const fnvPrime = 1099511628211

	for _, row := range rows {
		user := 0
		score := 0
		flag := false

		rowBytes := []byte(row)
		pos := 0
		for pos < len(rowBytes) {
			// Find the start of a key
			start := bytes.IndexByte(rowBytes[pos:], '"')
			if start == -1 {
				break
			}
			pos += start

			// Check for "user":
			if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 'u' && rowBytes[pos+2] == 's' && 
				rowBytes[pos+3] == 'e' && rowBytes[pos+4] == 'r' && rowBytes[pos+5] == '"' && 
				bytes.IndexByte(rowBytes[pos+6:], ':') != -1 {
				
					// Move to value
					valStart := bytes.IndexByte(rowBytes[pos+6:], ':') + pos + 6
					user = 0
					for i := valStart; i < len(rowBytes); i++ {
						c := rowBytes[i]
						if c >= '0' && c <= '9' {
							user = user*10 + int(c-'0')
					} else if c == ',' || c == '}' {
						break
					}
					}
					pos = valStart
			} else if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 's' && rowBytes[pos+2] == 'c' && 
				rowBytes[pos+3] == 'o' && rowBytes[pos+4] == 'r' && rowBytes[pos+5] == 'e' && 
				bytes.IndexByte(rowBytes[pos+6:], ':') != -1 {
				
					valStart := bytes.IndexByte(rowBytes[pos+6:], ':') + pos + 6
					score = 0
					for i := valStart; i < len(rowBytes); i++ {
						c := rowBytes[i]
						if c >= '0' && c <= '9' {
							score = score*10 + int(c-'0')
					} else if c == ',' || c == '}' {
						break
					}
					}
					pos = valStart
			} else if len(rowBytes)-pos >= 7 && 
				rowBytes[pos] == '"' && rowBytes[pos+1] == 'f' && rowBytes[pos+2] == 'l' && 
				rowBytes[pos+3] == 'a' && rowBytes[pos+4] == 'g' && rowBytes[pos+5] == '"' {
				
					valStart := bytes.IndexByte(rowBytes[pos+6:], ':') + pos + 6
					if len(rowBytes)-valStart >= 5 && 
						bytes.Index(rowBytes[valStart:], []byte("true")) != -1 {
						flag = true
					}
					pos = valStart
			} else {
				pos++
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