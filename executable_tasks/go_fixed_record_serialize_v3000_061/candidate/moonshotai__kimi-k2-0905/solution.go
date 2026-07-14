package main

import (
	"unsafe"
)

func solutionChecksumBytes(data []byte) uint64 {
	var h uint64 = 1469598103934665603
	for _, b := range data {
		h ^= uint64(b)
		h *= 1099511628211
	}
	return h
}

type record struct {
	id    uint32
	ts    uint32
	level uint16
	len   uint16
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	// build one contiguous slice of structs once
	recs := make([]record, n)
	for i := 0; i < n; i++ {
		recs[i] = record{id: ids[i], ts: ts[i], level: levels[i], len: lens[i]}
	}
	// view it as bytes without copying
	b := (*[1 << 30]byte)(unsafe.Pointer(&recs[0]))[:len(recs)*unsafe.Sizeof(recs[0])]
	var h uint64
	for i := 0; i < iters; i++ {
		h = solutionChecksumBytes(b)
	}
	return h
}