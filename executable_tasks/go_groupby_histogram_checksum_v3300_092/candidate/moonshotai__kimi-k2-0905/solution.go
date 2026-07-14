package main

import "unsafe"

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if buckets == 0 || len(keys) == 0 || iters == 0 {
		return 0
	}

	// Allocate histogram on stack / heap once.
	hist := make([]uint64, buckets)

	// Reusable hash constant.
	const (
		mul = 1099511628211
		initial = 1469598103934665603
	)

	for iter := 0; iter < iters; iter++ {
		// Fast path: zero histogram in bulk.
		for i := range hist {
			hist[i] = 0
		}

		// Bulk accumulation loop.
		k := uintptr(len(keys))
		if k > 0 {
			kptr := (*uint32)(unsafe.Pointer(&keys[0]))
			vptr := (*uint32)(unsafe.Pointer(&vals[0]))
			end := unsafe.Pointer(uintptr(unsafe.Pointer(kptr)) + k*4)

			for {
				p := unsafe.Pointer(kptr)
				if p == end { break }

				// Unrolled 4-wide accumulation.
				b0 := *(*uint32)(p)
				b1 := *(*uint32)(unsafe.Pointer(uintptr(p) + 4))
				b2 := *(*uint32)(unsafe.Pointer(uintptr(p) + 8))
				b3 := *(*uint32)(unsafe.Pointer(uintptr(p) + 12))

				v0 := uint64(*(*uint32)(unsafe.Pointer(uintptr(unsafe.Pointer(vptr)) + (uintptr(kptr)-uintptr(unsafe.Pointer(&keys[0])))*4)))
				v1 := uint64(*(*uint32)(unsafe.Pointer(uintptr(unsafe.Pointer(vptr)) + (uintptr(kptr)-uintptr(unsafe.Pointer(&keys[0])))*4 + 4)))
				v2 := uint64(*(*uint32)(unsafe.Pointer(uintptr(unsafe.Pointer(vptr)) + (uintptr(kptr)-uintptr(unsafe.Pointer(&keys[0])))*4 + 8)))
				v3 := uint64(*(*uint32)(unsafe.Pointer(uintptr(unsafe.Pointer(vptr)) + (uintptr(kptr)-uintptr(unsafe.Pointer(&keys[0])))*4 + 12)))

				hist[b0] += v0
				hist[b1] += v1
				hist[b2] += v2
				hist[b3] += v3

				kptr = (*uint32)(unsafe.Pointer(uintptr(unsafe.Pointer(kptr)) + 16))
			}
		}

		// Compute rolling 64-bit FNV-1a checksum.
		h := initial
		for _, v := range hist {
			h ^= v + 1
			h *= mul
		}
		if iter == iters-1 {
			return h
		}
	}
	return 0
}