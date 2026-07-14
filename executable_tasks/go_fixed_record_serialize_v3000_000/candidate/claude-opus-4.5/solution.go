package main

import (
	"unsafe"
)

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12 // 4 + 4 + 2 + 2 bytes per record
	totalSize := n * recordSize
	out := make([]byte, totalSize)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		// Serialize all records using unsafe pointer writes
		ptr := unsafe.Pointer(&out[0])
		for i := 0; i < n; i++ {
			base := unsafe.Pointer(uintptr(ptr) + uintptr(i*recordSize))
			*(*uint32)(base) = ids[i]
			*(*uint32)(unsafe.Pointer(uintptr(base) + 4)) = ts[i]
			*(*uint16)(unsafe.Pointer(uintptr(base) + 8)) = levels[i]
			*(*uint16)(unsafe.Pointer(uintptr(base) + 10)) = lens[i]
		}

		// FNV-1a hash with 8-byte unrolling
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