package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	var hash uint64
	const prime = 1099511628211
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i++ {
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			v = ts[i]
			hash ^= uint64(byte(v))
			hash *= prime
			hash ^= uint64(byte(v >> 8))
			hash *= prime
			hash ^= uint64(byte(v >> 16))
			hash *= prime
			hash ^= uint64(byte(v >> 24))
			hash *= prime

			v16 := levels[i]
			hash ^= uint64(byte(v16))
			hash *= prime
			hash ^= uint64(byte(v16 >> 8))
			hash *= prime

			v16 = lens[i]
			hash ^= uint64(byte(v16))
			hash *= prime
			hash ^= uint64(byte(v16 >> 8))
			hash *= prime
		}
	}
	return hash
}