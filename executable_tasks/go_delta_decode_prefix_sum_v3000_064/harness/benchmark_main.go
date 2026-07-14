package main

import (
	"encoding/json"
	"fmt"
	"time"
)

type result struct {
	OK               bool   `json:"ok"`
	ElapsedNS        int64  `json:"elapsed_ns"`
	ExpectedChecksum uint64 `json:"expected_checksum"`
	ActualChecksum   uint64 `json:"actual_checksum"`
}

func makeDeltas(n int) []int32 {
	deltas := make([]int32, n)
	var state uint64 = 0xCAFEBABE13572468
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		deltas[i] = int32((state>>24)&1023) - 320
	}
	return deltas
}

func expectedSum(deltas []int32) uint64 {
	var current int64
	var total uint64
	for _, d := range deltas {
		current += int64(d)
		total += uint64(current + 0x9E3779B1)
	}
	return total
}

func main() {
	deltas := makeDeltas(257600)
	expected := expectedSum(deltas)
	start := time.Now()
	actual := run(deltas, 33)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
}
