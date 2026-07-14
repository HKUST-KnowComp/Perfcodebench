package main

import "unsafe"

func run(ids, dictVals []uint32, iters int) uint64 {
	n := len(ids)
	if n == 0 {
		var h uint64
		for iter := 0; iter < iters; iter++ {
			h = 1469598103934665603
		}
		return h
	}

	var h uint64
	idsPtr := unsafe.Pointer(&ids[0])
	dictPtr := unsafe.Pointer(&dictVals[0])

	for iter := 0; iter < iters; iter++ {
		h = 1469598103934665603
		i := 0
		n8 := n - 7

		for ; i < n8; i += 8 {
			base := unsafe.Pointer(uintptr(idsPtr) + uintptr(i)*4)
			id0 := *(*uint32)(base)
			id1 := *(*uint32)(unsafe.Pointer(uintptr(base) + 4))
			id2 := *(*uint32)(unsafe.Pointer(uintptr(base) + 8))
			id3 := *(*uint32)(unsafe.Pointer(uintptr(base) + 12))
			id4 := *(*uint32)(unsafe.Pointer(uintptr(base) + 16))
			id5 := *(*uint32)(unsafe.Pointer(uintptr(base) + 20))
			id6 := *(*uint32)(unsafe.Pointer(uintptr(base) + 24))
			id7 := *(*uint32)(unsafe.Pointer(uintptr(base) + 28))

			v0 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id0)*4))
			v1 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id1)*4))
			v2 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id2)*4))
			v3 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id3)*4))
			v4 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id4)*4))
			v5 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id5)*4))
			v6 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id6)*4))
			v7 := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id7)*4))

			h ^= uint64(v0) + 1
			h *= 1099511628211
			h ^= uint64(v1) + 1
			h *= 1099511628211
			h ^= uint64(v2) + 1
			h *= 1099511628211
			h ^= uint64(v3) + 1
			h *= 1099511628211
			h ^= uint64(v4) + 1
			h *= 1099511628211
			h ^= uint64(v5) + 1
			h *= 1099511628211
			h ^= uint64(v6) + 1
			h *= 1099511628211
			h ^= uint64(v7) + 1
			h *= 1099511628211
		}

		for ; i < n; i++ {
			id := *(*uint32)(unsafe.Pointer(uintptr(idsPtr) + uintptr(i)*4))
			v := *(*uint32)(unsafe.Pointer(uintptr(dictPtr) + uintptr(id)*4))
			h ^= uint64(v) + 1
			h *= 1099511628211
		}
	}
	return h
}