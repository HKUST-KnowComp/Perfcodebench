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

func makeInput(n int, dictSize uint32) ([]uint32, []uint32) {
	ids := make([]uint32, n)
	dictVals := make([]uint32, dictSize)
	var state uint64 = 0x9E3779B97F4A7C15
	for i := uint32(0); i < dictSize; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		dictVals[i] = uint32((state >> 16) & 0xFFFFFF)
	}
	for i := 0; i < n; i++ {
		state = state*2862933555777941757 + 3037000493
		ids[i] = uint32((state >> 20) % uint64(dictSize))
	}
	return ids, dictVals
}

func expectedChecksum(ids, dictVals []uint32) uint64 {
	var h uint64 = 1469598103934665603
	for _, id := range ids {
		h ^= uint64(dictVals[id]) + 1
		h *= 1099511628211
	}
	return h
}

func main() {
	ids, dictVals := makeInput(488700, 896)
	expected := expectedChecksum(ids, dictVals)
	start := time.Now()
	actual := run(ids, dictVals, 11)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected { panic("checksum mismatch") }
}
