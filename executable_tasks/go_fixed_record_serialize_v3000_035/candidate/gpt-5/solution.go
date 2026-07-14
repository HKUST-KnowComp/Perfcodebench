package main

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	if iters <= 0 {
		return 0
	}
	const prime uint64 = 1099511628211
	h := uint64(1469598103934665603)

	n := len(ids)
	for i := 0; i < n; i++ {
		v := ids[i]
		h ^= uint64(byte(v))
		h *= prime
		h ^= uint64(byte(v >> 8))
		h *= prime
		h ^= uint64(byte(v >> 16))
		h *= prime
		h ^= uint64(byte(v >> 24))
		h *= prime

		v = ts[i]
		h ^= uint64(byte(v))
		h *= prime
		h ^= uint64(byte(v >> 8))
		h *= prime
		h ^= uint64(byte(v >> 16))
		h *= prime
		h ^= uint64(byte(v >> 24))
		h *= prime

		s := levels[i]
		h ^= uint64(byte(s))
		h *= prime
		h ^= uint64(byte(s >> 8))
		h *= prime

		s = lens[i]
		h ^= uint64(byte(s))
		h *= prime
		h ^= uint64(byte(s >> 8))
		h *= prime
	}
	return h
}
