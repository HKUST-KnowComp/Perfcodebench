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
	outLen := len(src)
	out := make([]byte, outLen)
	var hash uint64

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := src[base+3]
			alphaSrc := uint32(a)
			alphaDst := uint32(255 - a)

			out[base] = uint8((uint32(src[base])*alphaSrc + uint32(dst[base])*alphaDst + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*alphaSrc + uint32(dst[base+1])*alphaDst + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*alphaSrc + uint32(dst[base+2])*alphaDst + 127) / 255)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}