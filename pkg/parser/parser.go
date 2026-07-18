package parser

import (
	"errors"
	"nitipv5/dto"
	"strings"
)

func Parser(input string) (*dto.UserRequest, error) {

	parts := strings.Fields(input)
	if len(parts) <= 0 {
		return nil, errors.New("Command is not valid!")
	}

	return &dto.UserRequest{
		Name: strings.ToUpper(parts[0]),
		Args: parts[1:],
	}, nil
}
