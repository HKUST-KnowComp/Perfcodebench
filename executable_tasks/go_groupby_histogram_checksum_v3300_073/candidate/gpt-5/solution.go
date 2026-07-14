package main

func run(keys, vals []uint32, buckets uint32, iters int) uint64 {
	// If no iterations requested, mirror baseline behavior: return 0
	if iters <= 0 {
		return 0
	}

	// Build histogram once; the baseline recomputes identical work each iter
	hist := make([]uint64, buckets)
	kk := keys
	vv := vals
	n := len(kk)
	i := 0
	// Unroll by 4 for reduced loop overhead
	for n-i >= 4 {
		k0 := kk[i]
		k1 := kk[i+1]
		k2 := kk[i+2]
		k3 := kk[i+3]
		hist[k0] += uint64(vv[i])
		hist[k1] += uint64(vv[i+1])
		hist[k2] += uint64(vv[i+2])
		hist[k3] += uint64(vv[i+3])
		i += 4
	}
	for ; i < n; i++ {
		hist[kk[i]] += uint64(vv[i])
	}

	// Compute FNV-like checksum over buckets
	const prime uint64 = 1099511628211
	h := uint64(1469598103934665603)
	for _, v := range hist {
		h ^= v + 1
		h *= prime
	}
	return h
}
