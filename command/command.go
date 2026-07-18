package command

import "nitipv5/dto"

type CommandCenter interface {
	Action(req dto.UserRequest) (string, bool, error)
}

type ActionFunc func(req dto.UserRequest) (string, bool, error)
