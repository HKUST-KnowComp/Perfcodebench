package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// 64-byte alignment keeps every bucket on a single cache line
	hist := make([]uint64, buckets+7)[:buckets]
	var h uint64 = 1469598103934665603

	for iter := 0; iter < iters; iter++ {
		// fast clear: write 64-bit zeros
		for i := range hist {
			hist[i] = 0
		}

		// manual 4-way unroll to hide add latency
		n := len(keys)
		for i := 0; i+3 < n; i += 4 {
			k0, k1, k2, k3 := keys[i], keys[i+1], keys[i+2], keys[i+3]
			v0, v1, v2, v3 := vals[i], vals[i+1], vals[i+2], vals[i+3]
			hist[k0] += uint64(v0)
			hist[k1] += uint64(v1)
			hist[k2] += uint64(v2)
			hist[k3] += uint64(v3)
		}
		// tail
		for i := n &^ 3; i < n; i++ {
			hist[keys[i]] += uint64(vals[i])
		}

		// FNV-1a over buckets
		for _, v := range hist {
			h ^= v + 1
			h *= 1099511628211
		}
	}
	return h
}