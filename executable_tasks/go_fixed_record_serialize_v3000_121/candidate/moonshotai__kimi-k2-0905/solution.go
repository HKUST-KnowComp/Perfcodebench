package main

import (
	"unsafe"
)

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

type record struct {
	id    uint32
	ts    uint32
	level uint16
	len   uint16
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	// build a single slice of structs in one allocation
	recs := make([]record, n)
	for i := 0; i < n; i++ {
		recs[i] = record{id: ids[i], ts: ts[i], level: levels[i], len: lens[i]}
	}
	// view that slice as raw bytes
	b := (*[1 << 30]byte)(unsafe.Pointer(&recs[0]))[:len(recs)*unsafe.Sizeof(recs[0])]
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = solutionChecksumBytes(b)
	}
	return hash
}