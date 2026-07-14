package main

func solutionChecksumBytes(data []byte) uint64 {
	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)
	hash := offset
	for i := 0; i < len(data); i++ {
		hash ^= uint64(data[i])
		hash *= prime
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	n := pixels * 4
	out := make([]byte, n)

	const (
		offset uint64 = 1469598103934665603
		prime  uint64 = 1099511628211
	)

	var hash uint64
	for iter := 0; iter < iters; iter++ {
		h := offset
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a

			v := uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			out[i] = v
			h ^= uint64(v)
			h *= prime

			v = uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			out[i+1] = v
			h ^= uint64(v)
			h *= prime

			v = uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)
			out[i+2] = v
			h ^= uint64(v)
			h *= prime

			out[i+3] = 255
			h ^= 255
			h *= prime
		}
		hash = h
	}
	return hash
}
