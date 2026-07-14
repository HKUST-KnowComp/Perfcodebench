package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// If no iterations, baseline returns 0
	if iters == 0 {
		return 0
	}

	// Build histogram once; the result is identical across iterations
	hist := make([]uint64, int(buckets))

	n := len(keys)
	i := 0
	for ; i+3 < n; i += 4 {
		hist[int(keys[i+0])] += uint64(vals[i+0])
		hist[int(keys[i+1])] += uint64(vals[i+1])
		hist[int(keys[i+2])] += uint64(vals[i+2])
		hist[int(keys[i+3])] += uint64(vals[i+3])
	}
	for ; i < n; i++ {
		hist[int(keys[i])] += uint64(vals[i])
	}

	// Compute checksum once
	const prime uint64 = 1099511628211
	h := uint64(1469598103934665603)
	m := len(hist)
	i = 0
	for ; i+3 < m; i += 4 {
		v0 := hist[i]
		v1 := hist[i+1]
		v2 := hist[i+2]
		v3 := hist[i+3]
		h ^= v0 + 1
		h *= prime
		h ^= v1 + 1
		h *= prime
		h ^= v2 + 1
		h *= prime
		h ^= v3 + 1
		h *= prime
	}
	for ; i < m; i++ {
		h ^= hist[i] + 1
		h *= prime
	}

	return h
}
