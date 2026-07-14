package main

func solutionBlendByte(s, d, a uint8) uint8 {
	return uint8((uint32(s)*uint32(a) + uint32(d)*uint32(255-a) + 127) / 255)
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
	out := make([]byte, pixels*4)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i << 2
			a := src[base+3]
			if a == 255 {
				out[base] = src[base]
				out[base+1] = src[base+1]
				out[base+2] = src[base+2]
			} else if a == 0 {
				out[base] = dst[base]
				out[base+1] = dst[base+1]
				out[base+2] = dst[base+2]
			} else {
				ua := uint32(a)
				invA := 255 - ua
				out[base] = uint8((uint32(src[base])*ua + uint32(dst[base])*invA + 127) / 255)
				out[base+1] = uint8((uint32(src[base+1])*ua + uint32(dst[base+1])*invA + 127) / 255)
				out[base+2] = uint8((uint32(src[base+2])*ua + uint32(dst[base+2])*invA + 127) / 255)
			}
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}