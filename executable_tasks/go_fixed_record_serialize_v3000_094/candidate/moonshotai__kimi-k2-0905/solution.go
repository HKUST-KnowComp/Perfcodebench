package main

import "unsafe"

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	sz := n * 16
	out := make([]byte, 0, sz)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		out = out[:sz]
		dst := out
		for i := 0; i < n; i++ {
			*(*uint64)(unsafe.Pointer(&dst[0])) = uint64(ids[i]) | uint64(ts[i])<<32
			*(*uint64)(unsafe.Pointer(&dst[8])) = uint64(levels[i]) | uint64(lens[i])<<16
			dst = dst[16:]
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}