package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	n := pixels * 4
	if n > 0 {
		// Help bounds-check elimination for indexed accesses below
		_ = src[n-1]
		_ = dst[n-1]
	}

	const fnvOffset uint64 = 1469598103934665603
	const fnvPrime uint64 = 1099511628211

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := fnvOffset
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			inva := uint32(255) - a

			// R
			v := uint32(src[i])*a + uint32(dst[i])*inva
			b := uint8((v + 127) / 255)
			h ^= uint64(b)
			h *= fnvPrime

			// G
			v = uint32(src[i+1])*a + uint32(dst[i+1])*inva
			b = uint8((v + 127) / 255)
			h ^= uint64(b)
			h *= fnvPrime

			// B
			v = uint32(src[i+2])*a + uint32(dst[i+2])*inva
			b = uint8((v + 127) / 255)
			h ^= uint64(b)
			h *= fnvPrime

			// A is forced to 255 in output
			h ^= 255
			h *= fnvPrime
		}
		hash = h
	}
	return hash
}
