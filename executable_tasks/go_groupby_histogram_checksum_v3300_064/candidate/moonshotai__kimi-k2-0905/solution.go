package main

import (
	"math/bits"
	"unsafe"
)

//go:noescape
func memclrNoHeapPointers(ptr unsafe.Pointer, n uintptr)

const maxBuckets = 2000

var hist [maxBuckets]uint64

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	if buckets == 0 || buckets > maxBuckets {
		panic("bucket count out of allowed range")
	}
	bitsPerHash := bits.Len64(uint64(buckets))
	if 1<<bitsPerHash < int(buckets) {
		bitsPerHash++
	}
	mask := uint32((1 << bitsPerHash) - 1)
	if int(buckets) > (1<<bitsPerHash) {
		mask = buckets - 1
	}

	hbuf := hist[:buckets]
	var h uint64
	for iter := 0; iter < iters; iter++ {
		// fast bulk zero
		memclrNoHeapPointers(unsafe.Pointer(&hbuf[0]), uintptr(buckets)*unsafe.Sizeof(hbuf[0]))

		n := len(keys)
		i := 0
		// unroll 64-wide
		for n >= 64 {
			for j := 0; j < 64; j++ {
				idx := keys[i] & mask
				hist[idx] += uint64(vals[i])
				i++
			}
			n -= 64
		}
		for j := 0; j < n; j++ {
			idx := keys[i] & mask
			hist[idx] += uint64(vals[i])
			i++
		}

		// FNV-1a hash over histogram buffer
		h = 1469598103934665603
		for k := 0; k < len(hbuf); k += 4 {
			v0 := hbuf[k]
			h ^= v0 + 1
			h *= 1099511628211
			if k+1 < len(hbuf) {
				v1 := hbuf[k+1]
				h ^= v1 + 1
				h *= 1099511628211
			}
			if k+2 < len(hbuf) {
				v2 := hbuf[k+2]
				h ^= v2 + 1
				h *= 1099511628211
			}
			if k+3 < len(hbuf) {
				v3 := hbuf[k+3]
				h ^= v3 + 1
				h *= 1099511628211
			}
		}
	}
	return h
}