package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	hist := make([]uint64, buckets)
	var h uint64
	keyCount := len(keys)
	bucketCount := int(buckets)

	for iter := 0; iter < iters; iter++ {
		clear(hist)
		for i := 0; i < keyCount; i++ {
			hist[keys[i]] += uint64(vals[i])
		}
		h = 1469598103934665603
		for j := 0; j < bucketCount; j++ {
			v := hist[j]
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}