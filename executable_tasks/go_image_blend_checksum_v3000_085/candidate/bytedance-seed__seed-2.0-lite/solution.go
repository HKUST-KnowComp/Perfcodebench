package main

func solutionBlendByte(s, d, a uint8) uint8 {
	a32 := uint32(a)
	invA := 255 - a32
	sum := uint32(s)*a32 + uint32(d)*invA + 127
	return uint8((sum * 0x8081) >> 23)
}

func solutionChecksumBytes(data []byte) uint64 {
	var hash uint64 = 1469598103934665603
	for _, b := range data {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}

func run(src, dst []byte, width, height, iters int) uint64 {
	pixels := width * height
	out := make([]byte, 4*pixels)
	var hash uint64
	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			srcA := uint32(src[base+3])
			invA := 255 - srcA

			sumR := uint32(src[base])*srcA + uint32(dst[base])*invA + 127
			out[base] = uint8((sumR * 0x8081) >> 23)

			sumG := uint32(src[base+1])*srcA + uint32(dst[base+1])*invA + 127
			out[base+1] = uint8((sumG * 0x8081) >> 23)

			sumB := uint32(src[base+2])*srcA + uint32(dst[base+2])*invA + 127
			out[base+2] = uint8((sumB * 0x8081) >> 23)

			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}