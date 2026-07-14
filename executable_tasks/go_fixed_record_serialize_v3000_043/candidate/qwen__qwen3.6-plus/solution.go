package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const prime = 1099511628211
	const offset = 1469598103934665603
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = offset
		for i := range ids {
			v := ids[i]
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime

			v = ts[i]
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime
			v >>= 8
			hash ^= uint64(v & 0xFF); hash *= prime

			v16 := levels[i]
			hash ^= uint64(v16 & 0xFF); hash *= prime
			v16 >>= 8
			hash ^= uint64(v16 & 0xFF); hash *= prime

			v16 = lens[i]
			hash ^= uint64(v16 & 0xFF); hash *= prime
			v16 >>= 8
			hash ^= uint64(v16 & 0xFF); hash *= prime
		}
	}
	return hash
}