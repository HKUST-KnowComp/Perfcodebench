package main

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	const offset = 1469598103934665603
	const prime = 1099511628211

	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := uint64(offset)
		for i := 0; i < n; i++ {
			// ids[i] – 4 bytes little‑endian
			v := ids[i]
			h ^= uint64(byte(v))
			h *= prime
			h ^= uint64(byte(v >> 8))
			h *= prime
			h ^= uint64(byte(v >> 16))
			h *= prime
			h ^= uint64(byte(v >> 24))
			h *= prime

			// ts[i] – 4 bytes little‑endian
			v = ts[i]
			h ^= uint64(byte(v))
			h *= prime
			h ^= uint64(byte(v >> 8))
			h *= prime
			h ^= uint64(byte(v >> 16))
			h *= prime
			h ^= uint64(byte(v >> 24))
			h *= prime

			// levels[i] – 2 bytes little‑endian
			w := levels[i]
			h ^= uint64(byte(w))
			h *= prime
			h ^= uint64(byte(w >> 8))
			h *= prime

			// lens[i] – 2 bytes little‑endian
			w = lens[i]
			h ^= uint64(byte(w))
			h *= prime
			h ^= uint64(byte(w >> 8))
			h *= prime
		}
		hash = h
	}
	return hash
}
