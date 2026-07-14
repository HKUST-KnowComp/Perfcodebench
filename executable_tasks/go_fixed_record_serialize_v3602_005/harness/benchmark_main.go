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

func checksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func putU32(dst []byte, v uint32) {
	dst[0] = byte(v)
	dst[1] = byte(v >> 8)
	dst[2] = byte(v >> 16)
	dst[3] = byte(v >> 24)
}

func putU16(dst []byte, v uint16) {
	dst[0] = byte(v)
	dst[1] = byte(v >> 8)
}

func makeInput(n int) ([]uint32, []uint32, []uint16, []uint16) {
	ids := make([]uint32, n)
	ts := make([]uint32, n)
	levels := make([]uint16, n)
	lens := make([]uint16, n)
	var state uint64 = 0x13579BDF2468ACE0
	for i := 0; i < n; i++ {
		state = state*6364136223846793005 + 1442695040888963407
		ids[i] = uint32(state)
		state = state*2862933555777941757 + 3037000493
		ts[i] = uint32(state)
		levels[i] = uint16((state >> 16) & 7)
		lens[i] = uint16(24 + ((state >> 24) & 255))
	}
	return ids, ts, levels, lens
}

func expectedChecksum(ids, ts []uint32, levels, lens []uint16) uint64 {
	out := make([]byte, len(ids)*12)
	ptr := 0
	for i := range ids {
		putU32(out[ptr:], ids[i]); ptr += 4
		putU32(out[ptr:], ts[i]); ptr += 4
		putU16(out[ptr:], levels[i]); ptr += 2
		putU16(out[ptr:], lens[i]); ptr += 2
	}
	return checksumBytes(out)
}

func main() {
	ids, ts, levels, lens := makeInput(333800)
	expected := expectedChecksum(ids, ts, levels, lens)
	start := time.Now()
	actual := run(ids, ts, levels, lens, 32)
	elapsed := time.Since(start).Nanoseconds()
	out, _ := json.Marshal(result{OK: actual == expected, ElapsedNS: elapsed, ExpectedChecksum: expected, ActualChecksum: actual})
	fmt.Println(string(out))
	if actual != expected {
		panic("checksum mismatch")
	}
}
