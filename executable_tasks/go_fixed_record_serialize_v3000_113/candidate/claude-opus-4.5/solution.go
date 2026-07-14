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
			offset := uintptr(i * recordSize)
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + offset)) = ids[i]
			*(*uint32)(unsafe.Pointer(uintptr(ptr) + offset + 4)) = ts[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + offset + 8)) = levels[i]
			*(*uint16)(unsafe.Pointer(uintptr(ptr) + offset + 10)) = lens[i]
		}
		hash = fnv1aHash(out)
	}
	return hash
}

func fnv1aHash(data []byte) uint64 {
	hash := uint64(1469598103934665603)
	const prime = 1099511628211

	n := len(data)
	i := 0

	for ; i+7 < n; i += 8 {
		hash ^= uint64(data[i])
		hash *= prime
		hash ^= uint64(data[i+1])
		hash *= prime
		hash ^= uint64(data[i+2])
		hash *= prime
		hash ^= uint64(data[i+3])
		hash *= prime
		hash ^= uint64(data[i+4])
		hash *= prime
		hash ^= uint64(data[i+5])
		hash *= prime
		hash ^= uint64(data[i+6])
		hash *= prime
		hash ^= uint64(data[i+7])
		hash *= prime
	}

	for ; i < n; i++ {
		hash ^= uint64(data[i])
		hash *= prime
	}

	return hash
}