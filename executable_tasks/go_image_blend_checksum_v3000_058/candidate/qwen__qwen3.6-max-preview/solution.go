package main

func run(src, dst []byte, width, height, iters int) uint64 {
	n := width * height * 4
	out := make([]byte, n)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < n; i += 4 {
			a := uint32(src[i+3])
			invA := 255 - a

			r := uint8((uint32(src[i])*a + uint32(dst[i])*invA + 127) / 255)
			out[i] = r
			hash ^= uint64(r)
			hash *= 1099511628211

			g := uint8((uint32(src[i+1])*a + uint32(dst[i+1])*invA + 127) / 255)
			out[i+1] = g
			hash ^= uint64(g)
			hash *= 1099511628211

			b := uint8((uint32(src[i+2])*a + uint32(dst[i+2])*invA + 127) / 255)
			out[i+2] = b
			hash ^= uint64(b)
			hash *= 1099511628211

			out[i+3] = 255
			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}
