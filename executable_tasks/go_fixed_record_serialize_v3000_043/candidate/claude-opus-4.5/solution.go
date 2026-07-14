package main

import "unsafe"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	if n == 0 {
		var hash uint64 = 1469598103934665603
		return hash
	}

	// Pre-allocate exact size: 12 bytes per record
	bufSize := n * 12
	out := make([]byte, bufSize)

	var hash uint64

	for iter := 0; iter < iters; iter++ {
		// Serialize all records using unsafe for speed
		ptr := unsafe.Pointer(&out[0])
		off := uintptr(0)

		// Process 4 records at a time for better ILP
		i := 0
		for ; i+3 < n; i += 4 {
			// Record 0
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off)) = ids[i]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 4)) = ts[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 8)) = levels[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 10)) = lens[i]
			// Record 1
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 12)) = ids[i+1]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 16)) = ts[i+1]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 20)) = levels[i+1]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 22)) = lens[i+1]
			// Record 2
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 24)) = ids[i+2]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 28)) = ts[i+2]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 32)) = levels[i+2]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 34)) = lens[i+2]
			// Record 3
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 36)) = ids[i+3]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 40)) = ts[i+3]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 44)) = levels[i+3]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 46)) = lens[i+3]
			off += 48
		}
		// Handle remaining records
		for ; i < n; i++ {
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off)) = ids[i]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + off + 4)) = ts[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 8)) = levels[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + off + 10)) = lens[i]
			off += 12
		}

		// FNV-1a hash with unrolling
		hash = 1469598103934665603
		const prime = 1099511628211

		// Process 8 bytes at a time
		j := 0
		for ; j+7 < bufSize; j += 8 {
			hash ^= uint64(out[j])
			hash *= prime
			hash ^= uint64(out[j+1])
			hash *= prime
			hash ^= uint64(out[j+2])
			hash *= prime
			hash ^= uint64(out[j+3])
			hash *= prime
			hash ^= uint64(out[j+4])
			hash *= prime
			hash ^= uint64(out[j+5])
			hash *= prime
			hash ^= uint64(out[j+6])
			hash *= prime
			hash ^= uint64(out[j+7])
			hash *= prime
		}
		// Handle remaining bytes
		for ; j < bufSize; j++ {
			hash ^= uint64(out[j])
			hash *= prime
		}
	}

	return hash
}