package server

import (
	"bufio"
	"fmt"
	"log"
	"net"
	"nitipv5/command"
	"nitipv5/pkg/parser"
)

type Cli struct {
	cc command.CommandCenter
}

func NewCLI(cc command.CommandCenter) *Cli {
	return &Cli{
		cc: cc,
	}
}

func (c *Cli) Start() {
	// open connection
	port := ":5000"
	listener, err := net.Listen("tcp", port)
	if err != nil {
		log.Fatalf("Something went wrong %s", err.Error())
	}

	fmt.Printf("NitipV5 CLI is running on purt %s\n", port)

	// listen for user connected
	for {

		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("User failure to connect %s", err.Error())
			continue
		}
		fmt.Printf("New user connected from %s", conn.RemoteAddr().String())

		go c.Handler(conn)
	}
}

func (c *Cli) Handler(conn net.Conn) {
	defer conn.Close()

	scanner := bufio.NewScanner(conn)
	conn.Write([]byte("Welcome to nitipv5!\n"))

	for {
		conn.Write([]byte("> "))
		if !scanner.Scan() {
			continue
		}

		// parsing user input
		userRequest, err := parser.Parser(scanner.Text())
		if err != nil {
			conn.Write([]byte(err.Error() + "\n"))
			continue
		}

		// send to command center
		msg, stop, err := c.cc.Action(*userRequest)
		if err != nil {
			conn.Write([]byte(err.Error() + "\n"))
		} else {
			conn.Write([]byte(msg + "\n"))
		}

		if stop {
			break
		}
	}
}
