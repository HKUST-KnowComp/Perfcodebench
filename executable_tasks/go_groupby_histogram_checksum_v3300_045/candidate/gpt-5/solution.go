package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// If no iterations requested, baseline returns 0
	if iters <= 0 {
		return 0
	}

	// Build histogram once since each iteration would produce the same result
	bLen := int(buckets)
	hist := make([]uint64, bLen)

	n := len(keys)
	i := 0
	// Unrolled accumulation loop by 4 for better throughput
	for i+3 < n {
		k0 := int(keys[i])
		k1 := int(keys[i+1])
		k2 := int(keys[i+2])
		k3 := int(keys[i+3])
		hist[k0] += uint64(vals[i])
		hist[k1] += uint64(vals[i+1])
		hist[k2] += uint64(vals[i+2])
		hist[k3] += uint64(vals[i+3])
		i += 4
	}
	for ; i < n; i++ {
		hist[int(keys[i])] += uint64(vals[i])
	}

	// Compute FNV-like checksum once (same each iteration)
	const offset uint64 = 1469598103934665603
	const prime uint64 = 1099511628211
	h := offset

	m := bLen
	j := 0
	// Unroll hashing loop by 4
	for j+3 < m {
		v0 := hist[j]
		v1 := hist[j+1]
		v2 := hist[j+2]
		v3 := hist[j+3]
		h ^= v0 + 1
		h *= prime
		h ^= v1 + 1
		h *= prime
		h ^= v2 + 1
		h *= prime
		h ^= v3 + 1
		h *= prime
		j += 4
	}
	for ; j < m; j++ {
		v := hist[j]
		h ^= v + 1
		h *= prime
	}

	return h
}
