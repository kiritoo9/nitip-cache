package main

import (
	"nitipv5/command"
	"nitipv5/server"
	"nitipv5/storage"
)

func main() {
	// prepare server
	storage := storage.NewStorageMemory()
	commandCenter := command.NewCommandCenter(storage)

	server := server.NewCLI(commandCenter)

	server.Start()

}
