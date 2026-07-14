package main

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			invA := uint32(255 - a)

			// Blend R
			v0 := uint32(src[base])*uint32(a) + uint32(dst[base])*invA + 127
			hash ^= uint64((v0 * 0x8081) >> 23)
			hash *= 1099511628211

			// Blend G
			v1 := uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*invA + 127
			hash ^= uint64((v1 * 0x8081) >> 23)
			hash *= 1099511628211

			// Blend B
			v2 := uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*invA + 127
			hash ^= uint64((v2 * 0x8081) >> 23)
			hash *= 1099511628211

			// Alpha channel is always 255
			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}
