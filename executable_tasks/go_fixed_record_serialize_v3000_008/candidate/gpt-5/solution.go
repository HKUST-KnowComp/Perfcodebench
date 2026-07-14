package main

const (
	fnvOffset uint64 = 1469598103934665603
	fnvPrime  uint64 = 1099511628211
)

func solutionChecksumBytes(data []byte) uint64 {
	h := fnvOffset
	for _, b := range data {
		h ^= uint64(b)
		h *= fnvPrime
	}
	return h
}

func run(ids, ts []uint32, levels, lens []uint16, iters int) uint64 {
	n := len(ids)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i++ {
			v := ids[i]
			b := byte(v)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 8)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 16)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 24)
			h ^= uint64(b)
			h *= fnvPrime

			v = ts[i]
			b = byte(v)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 8)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 16)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(v >> 24)
			h ^= uint64(b)
			h *= fnvPrime

			w := levels[i]
			b = byte(w)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(w >> 8)
			h ^= uint64(b)
			h *= fnvPrime

			w = lens[i]
			b = byte(w)
			h ^= uint64(b)
			h *= fnvPrime
			b = byte(w >> 8)
			h ^= uint64(b)
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
