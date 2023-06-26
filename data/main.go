package main

import (
	"fmt"
	"io"
	"os"

	"github.com/pion/webrtc/v3/pkg/media/h264reader"
)

func main() {
	file, err := os.Open("../dump")
	if err != nil {
		panic(err)
	}

	h264, err := h264reader.NewReader(file)
	if err != nil {
		panic(err)
	}

	writeNalFile := func(id int, data []byte) error {
		data = append([]byte{0x00, 0x00, 0x00, 0x01}, data...)

		filename := fmt.Sprintf("dump.%04d", id)
		fd, err := os.OpenFile(filename, os.O_CREATE|os.O_RDWR, 0644)
		if err != nil {
			return err
		}
		defer fd.Close()

		_, err = fd.Write(data)
		return err
	}

	for i := 0; ; i = i + 1 {
		nal, h264Err := h264.NextNAL()
		if h264Err == io.EOF {
			break
		}
		fmt.Println(nal.UnitType.String())
		writeNalFile(i, nal.Data)
	}

}
