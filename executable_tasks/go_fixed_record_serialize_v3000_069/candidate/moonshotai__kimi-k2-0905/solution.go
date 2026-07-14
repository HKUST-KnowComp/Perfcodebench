package main

import (
	"unsafe"
)

func solutionChecksumBytes(data []byte) uint64 {
	h := uint64(1469598103934665603)
	i := 0
	n := len(data)
	for n >= 4 {
		h ^= uint64(data[i])
		h *= 1099511628211
		h ^= uint64(data[i+1])
		h *= 1099511628211
		h ^= uint64(data[i+2])
		h *= 1099511628211
		h ^= uint64(data[i+3])
		h *= 1099511628211
		i += 4
		n -= 4
	}
	for ; n > 0; n-- {
		h ^= uint64(data[i])
		h *= 1099511628211
		i++
	}
	return h
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	total := n * 12
	out := make([]byte, total)
	hash := uint64(0)
	for iter := 0; iter < iters; iter++ {
		wp := 0
		for i := 0; i < n; i++ {
			*(*uint32)(unsafe.Pointer(&out[wp])) = ids[i]
			wp += 4
			*(*uint32)(unsafe.Pointer(&out[wp])) = ts[i]
			wp += 4
			*(*uint16)(unsafe.Pointer(&out[wp])) = levels[i]
			wp += 2
			*(*uint16)(unsafe.Pointer(&out[wp])) = lens[i]
			wp += 2
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}