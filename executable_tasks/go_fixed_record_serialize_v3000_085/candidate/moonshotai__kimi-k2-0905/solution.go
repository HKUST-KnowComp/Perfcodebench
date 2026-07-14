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
	// one allocation for the whole block
	buf := make([]record, n)
	for i := 0; i < n; i++ {
		buf[i] = record{
			id:    ids[i],
			ts:    ts[i],
			level: levels[i],
			len:   lens[i],
		}
	}
	// unsafe cast to byte slice
	b := (*[1 << 30]byte)(unsafe.Pointer(&buf[0]))[:n*unsafe.Sizeof(buf[0]):n*unsafe.Sizeof(buf[0])]
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = solutionChecksumBytes(b)
	}
	return hash
}