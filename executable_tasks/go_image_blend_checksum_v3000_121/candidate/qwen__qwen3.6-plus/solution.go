package main

func run(src, dst []byte, width, height, iters int) uint64 {
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for j := 0; j < len(src); j += 4 {
			a := uint32(src[j+3])
			invA := 255 - a

			r := uint32(src[j])*a + uint32(dst[j])*invA
			r = (r*257 + 32768) >> 16
			hash ^= uint64(r)
			hash *= 1099511628211

			g := uint32(src[j+1])*a + uint32(dst[j+1])*invA
			g = (g*257 + 32768) >> 16
			hash ^= uint64(g)
			hash *= 1099511628211

			b := uint32(src[j+2])*a + uint32(dst[j+2])*invA
			b = (b*257 + 32768) >> 16
			hash ^= uint64(b)
			hash *= 1099511628211

			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}