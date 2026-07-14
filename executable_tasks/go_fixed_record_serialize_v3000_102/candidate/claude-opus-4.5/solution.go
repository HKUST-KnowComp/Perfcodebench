package main

import "unsafe"

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	recordSize := 12
	totalSize := n * recordSize
	out := make([]byte, totalSize)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		ptr := unsafe.Pointer(&out[0])
		for i := 0; i < n; i++ {
			base := unsafe.Pointer(uintptr(ptr) + uintptr(i*recordSize))
			*(*uint32)(base) = ids[i]
			*(*uint32)(unsafe.Pointer(uintptr(base) + 4)) = ts[i]
			*(*uint16)(unsafe.Pointer(uintptr(base) + 8)) = levels[i]
			*(*uint16)(unsafe.Pointer(uintptr(base) + 10)) = lens[i]
		}
		hash = fnv1aFast(out)
	}
	return hash
}

func fnv1aFast(data []byte) uint64 {
	hash := uint64(1469598103934665603)
	n := len(data)
	i := 0

	if n >= 8 {
		ptr := unsafe.Pointer(&data[0])
		for ; i <= n-8; i += 8 {
			v := *(*uint64)(unsafe.Pointer(uintptr(ptr) + uintptr(i)))
			hash ^= v & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 8) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 16) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 24) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 32) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 40) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 48) & 0xFF
			hash *= 1099511628211
			hash ^= (v >> 56) & 0xFF
			hash *= 1099511628211
		}
	}

	for ; i < n; i++ {
		hash ^= uint64(data[i])
		hash *= 1099511628211
	}
	return hash
}