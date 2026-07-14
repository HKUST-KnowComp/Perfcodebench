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
		data := []byte(row)
		user := 0
		score := 0
		flag := false

		for i := 0; i < len(data); {
			// We look for the start of a key (quote)
			if data[i] == '"' {
				// Check for "user":
				if i+len(userKey)-1 < len(data) && bytes.Equal(data[i:i+len(userKey)], userKey) {
					i += len(userKey)
					for i < len(data) && (data[i] >= '0' && data[i] <= '9') {
						user = user*10 + int(data[i]-'0')
						i++
					}
					continue
				}
				// Check for "score":
				if i+len(scoreKey)-1 < len(data) && bytes.Equal(data[i:i+len(scoreKey)], scoreKey) {
					i += len(scoreKey)
					for i < len(data) && (data[i] >= '0' && data[i] <= '9') {
						score = score*10 + int(data[i]-'0')
						i++
					}
					continue
				}
				// Check for "flag":true
				if i+len(flagTrue)-1 < len(data) && bytes.Equal(data[i:i+len(flagTrue)], flagTrue) {
					flag = true
					i += len(flagTrue)
					continue
				}
			}
			i++
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