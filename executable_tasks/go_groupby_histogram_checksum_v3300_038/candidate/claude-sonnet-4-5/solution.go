package main

import "unsafe"

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	
	for iter := 0; iter < iters; iter++ {
		// Fast zero using unsafe pointer arithmetic
		ptr := unsafe.Pointer(&hist[0])
		n := int(buckets)
		i := 0
		for ; i+7 < n; i += 8 {
			*(*uint64)(unsafe.Add(ptr, i*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+1)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+2)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+3)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+4)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+5)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+6)*8)) = 0
			*(*uint64)(unsafe.Add(ptr, (i+7)*8)) = 0
		}
		for ; i < n; i++ {
			hist[i] = 0
		}
		
		// Unrolled aggregation loop
		len := len(keys)
		i = 0
		for ; i+3 < len; i += 4 {
			hist[keys[i]] += uint64(vals[i])
			hist[keys[i+1]] += uint64(vals[i+1])
			hist[keys[i+2]] += uint64(vals[i+2])
			hist[keys[i+3]] += uint64(vals[i+3])
		}
		for ; i < len; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		
		// Unrolled checksum loop
		h = 1469598103934665603
		i = 0
		for ; i+1 < n; i += 2 {
			v0 := hist[i]
			v1 := hist[i+1]
			h ^= v0 + 1
			h *= 1099511628211
			h ^= v1 + 1
			h *= 1099511628211
		}
		for ; i < n; i++ {
			h ^= hist[i] + 1
			h *= 1099511628211
		}
	}
	return h
}