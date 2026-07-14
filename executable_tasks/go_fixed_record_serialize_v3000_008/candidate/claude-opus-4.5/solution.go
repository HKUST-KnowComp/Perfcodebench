package main

import "unsafe"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12
	totalSize := n * recordSize
	out := make([]byte, totalSize)
	
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using unsafe pointer writes
		for i := 0; i < n; i++ {
			off := i * recordSize
			*(*uint32)(unsafe.Pointer(&out[off])) = ids[i]
			*(*uint32)(unsafe.Pointer(&out[off+4])) = ts[i]
			*(*uint16)(unsafe.Pointer(&out[off+8])) = levels[i]
			*(*uint16)(unsafe.Pointer(&out[off+10])) = lens[i]
		}
		
		// FNV-1a hash with unrolling
		hash = 1469598103934665603
		i := 0
		for ; i+7 < totalSize; i += 8 {
			hash ^= uint64(out[i])
			hash *= 1099511628211
			hash ^= uint64(out[i+1])
			hash *= 1099511628211
			hash ^= uint64(out[i+2])
			hash *= 1099511628211
			hash ^= uint64(out[i+3])
			hash *= 1099511628211
			hash ^= uint64(out[i+4])
			hash *= 1099511628211
			hash ^= uint64(out[i+5])
			hash *= 1099511628211
			hash ^= uint64(out[i+6])
			hash *= 1099511628211
			hash ^= uint64(out[i+7])
			hash *= 1099511628211
		}
		for ; i < totalSize; i++ {
			hash ^= uint64(out[i])
			hash *= 1099511628211
		}
	}
	return hash
}