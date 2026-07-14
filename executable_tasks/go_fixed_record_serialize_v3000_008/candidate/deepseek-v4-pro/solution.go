package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const (
		initHash uint64 = 1469598103934665603
		mult     uint64 = 1099511628211
	)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = initHash
		for i := range ids {
			v := ids[i]
			hash ^= uint64(byte(v))
			hash *= mult
			hash ^= uint64(byte(v >> 8))
			hash *= mult
			hash ^= uint64(byte(v >> 16))
			hash *= mult
			hash ^= uint64(byte(v >> 24))
			hash *= mult

			v2 := ts[i]
			hash ^= uint64(byte(v2))
			hash *= mult
			hash ^= uint64(byte(v2 >> 8))
			hash *= mult
			hash ^= uint64(byte(v2 >> 16))
			hash *= mult
			hash ^= uint64(byte(v2 >> 24))
			hash *= mult

			v3 := levels[i]
			hash ^= uint64(byte(v3))
			hash *= mult
			hash ^= uint64(byte(v3 >> 8))
			hash *= mult

			v4 := lens[i]
			hash ^= uint64(byte(v4))
			hash *= mult
			hash ^= uint64(byte(v4 >> 8))
			hash *= mult
		}
	}
	return hash
}
