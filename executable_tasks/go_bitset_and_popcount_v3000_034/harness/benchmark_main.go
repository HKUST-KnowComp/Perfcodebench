package main

import (
	"encoding/json"
	"fmt"
	"math/bits"
	"time"
)

type result struct {
	OK               bool   `json:"ok"`
	ElapsedNS        int64  `json:"elapsed_ns"`
	ExpectedChecksum uint64 `json:"expected_checksum"`
	ActualChecksum   uint64 `json:"actual_checksum"`
}

func makeWords(n int) ([]uint64, []uint64) {
	left := make([]uint64, n)
	right := make([]uint64, n)
	var state uint64 = 0xDEADBEEF12345678
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		left[i] = state ^ (state << 7)
		state = state*2862933555777941757 + 3037000493
		right[i] = state ^ (state >> 11)
	}
	return left, right
}

func expectedCount(left, right []uint64) uint64 {
	var total uint64
	for i := range left {
		total += uint64(bits.OnesCount64(left[i] & right[i]))
	}
	return total
}

func main() {
	left, right := makeWords(59800)
	expected := expectedCount(left, right)
	start := time.Now()
	actual := run(left, right, 36)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
}
