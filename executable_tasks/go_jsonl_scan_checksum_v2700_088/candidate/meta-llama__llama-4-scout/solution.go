package main

import (
	"encoding/json"
	"strings"
)

type jsonlRow struct {
	User  int    `json:"user"`
	Score int    `json:"score"`
	Flag  bool   `json:"flag"`
}

func run(rows []string) uint64 {
	var hash uint64 = 1469598103934665603
	for _, row := range rows {
		var jr jsonlRow
		err := json.Unmarshal([]byte(row), &jr)
		if err != nil {
			// Handle error, for simplicity, we assume the input is valid
			continue
		}
		v := uint64((jr.User & 1023) + jr.Score)
		if jr.Flag {
			v += 17
		}
		hash ^= v + 1
		hash *= 1099511628211
	}
	return hash
}