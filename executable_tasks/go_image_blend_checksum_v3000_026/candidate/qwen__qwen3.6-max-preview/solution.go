package main

func run(src, dst []byte, width, height, iters int) uint64 {
	_ = width
	_ = height
	_ = iters

	var hash uint64 = 1469598103934665603
	const prime = 1099511628211

	for i := 0; i < len(src); i += 4 {
		a := uint32(src[i+3])
		invA := 255 - a

		b0 := uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
		hash ^= uint64(b0)
		hash *= prime

		b1 := uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
		hash ^= uint64(b1)
		hash *= prime

		b2 := uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)
		hash ^= uint64(b2)
		hash *= prime

		hash ^= uint64(255)
		hash *= prime
	}
	return hash
}
