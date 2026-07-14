package main

func run(src, dst []byte, width, height, iters int) uint64 {
	if iters == 0 {
		return 0
	}
	pixels := width * height
	outLen := pixels * 4
	out := make([]byte, outLen)

	for iter := 0; iter < iters; iter++ {
		for i := 0; i < pixels; i++ {
			base := i * 4
			a := uint32(src[base+3])
			invA := 255 - a

			sR, sG, sB := uint32(src[base]), uint32(src[base+1]), uint32(src[base+2])
			dR, dG, dB := uint32(dst[base]), uint32(dst[base+1]), uint32(dst[base+2])

			out[base] = byte((sR*a + dR*invA + 127) / 255)
			out[base+1] = byte((sG*a + dG*invA + 127) / 255)
			out[base+2] = byte((sB*a + dB*invA + 127) / 255)
			out[base+3] = 255
		}
	}

	var hash uint64 = 1469598103934665603
	for _, b := range out {
		hash ^= uint64(b)
		hash *= 1099511628211
	}
	return hash
}
