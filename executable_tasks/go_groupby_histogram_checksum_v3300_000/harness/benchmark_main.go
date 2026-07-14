package main

import (
	"encoding/json"
	"fmt"
	"time"
)

type result struct {
	OK bool `json:"ok"`
	ElapsedNS int64 `json:"elapsed_ns"`
	ExpectedChecksum uint64 `json:"expected_checksum"`
	ActualChecksum uint64 `json:"actual_checksum"`
}

func makeInput(n int, buckets uint32) ([]uint32, []uint32) {
	keys := make([]uint32, n)
	vals := make([]uint32, n)
	var state uint64 = 0xA55A5AA55A5AA55A
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		keys[i] = uint32((state >> 16) % uint64(buckets))
		vals[i] = 1 + uint32((state>>40)&31)
	}
	return keys, vals
}

func expectedChecksum(keys, vals []uint32, buckets uint32) uint64 {
	hist := make([]uint64, buckets)
	for i := range keys { hist[keys[i]] += uint64(vals[i]) }
	var h uint64 = 1469598103934665603
	for _, v := range hist {
		h ^= v + 1
		h *= 1099511628211
	}
	return h
}

func main() {
	keys, vals := makeInput(12000, 128)
	expected := expectedChecksum(keys, vals, 128)
	start := time.Now()
	actual := run(keys, vals, 128, 8)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected { panic("checksum mismatch") }
}
