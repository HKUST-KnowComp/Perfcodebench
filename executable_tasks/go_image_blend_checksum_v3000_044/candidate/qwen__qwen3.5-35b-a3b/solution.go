package main

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
			base := i * 4
			a := src[base+3]
			out[base] = uint8((uint32(src[base])*uint32(a) + uint32(dst[base])*uint32(255-a) + 127) / 255)
			out[base+1] = uint8((uint32(src[base+1])*uint32(a) + uint32(dst[base+1])*uint32(255-a) + 127) / 255)
			out[base+2] = uint8((uint32(src[base+2])*uint32(a) + uint32(dst[base+2])*uint32(255-a) + 127) / 255)
			out[base+3] = 255
		}
		hash = solutionChecksumBytes(out)
	}
	return hash
}