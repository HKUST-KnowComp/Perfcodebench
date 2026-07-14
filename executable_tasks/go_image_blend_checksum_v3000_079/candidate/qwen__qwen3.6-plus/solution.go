package main

func run(src, dst []byte, width, height, iters int) uint64 {
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		hash = 1469598103934665603
		for i := 0; i < len(src); i += 4 {
			a := src[i+3]
			invA := 255 - a
			ua := uint32(a)
			uinvA := uint32(invA)

			v := uint32(src[i])*ua + uint32(dst[i])*uinvA + 127
			hash ^= uint64(v / 255)
			hash *= 1099511628211

			v = uint32(src[i+1])*ua + uint32(dst[i+1])*uinvA + 127
			hash ^= uint64(v / 255)
			hash *= 1099511628211

			v = uint32(src[i+2])*ua + uint32(dst[i+2])*uinvA + 127
			hash ^= uint64(v / 255)
			hash *= 1099511628211

			hash ^= 255
			hash *= 1099511628211
		}
	}
	return hash
}