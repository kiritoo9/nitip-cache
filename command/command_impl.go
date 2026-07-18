package command

import (
	"errors"
	"fmt"
	"nitipv5/dto"
	"nitipv5/storage"
	"strconv"
	"time"
)

type Command struct {
	Storage storage.Storage

	actions map[string]ActionFunc
}

func NewCommandCenter(storage storage.Storage) CommandCenter {
	c := &Command{
		Storage: storage,
		actions: make(map[string]ActionFunc),
	}

	c.actions["GET"] = c.actionGet
	c.actions["SET"] = c.actionSet
	c.actions["EXP"] = c.actionExp
	c.actions["DEL"] = c.actionDel
	c.actions["EXIT"] = c.actionExit

	return c
}

func (c *Command) Action(req dto.UserRequest) (string, bool, error) {
	action, ok := c.actions[req.Name]
	if !ok {
		return "Unknown command", false, nil
	}

	return action(req)
}

func (c *Command) actionGet(req dto.UserRequest) (string, bool, error) {
	if len(req.Args) < 1 {
		return "", false, errors.New("Usage: GET key")
	}

	item, err := c.Storage.Get(req.Args[0])
	if err != nil {
		return "", false, err
	}

	expiredAt := "never"
	if item.ExpiredAt != nil {
		expiredAt = time.Until(*item.ExpiredAt).String()
	}

	return fmt.Sprintf("%s | Expired At: %s", item.Value, expiredAt), false, nil
}

func (c *Command) actionSet(req dto.UserRequest) (string, bool, error) {
	if len(req.Args) < 2 {
		return "", false, errors.New("Usage: SET key value")
	}

	c.Storage.Set(req.Args[0], req.Args[1])
	return "OK!", false, nil
}

func (c *Command) actionExp(req dto.UserRequest) (string, bool, error) {
	if len(req.Args) < 2 {
		return "", false, errors.New("Usage: EXP key duration")
	}

	duration, err := strconv.Atoi(req.Args[1])
	if err != nil {
		return "", false, err
	}

	err = c.Storage.Exp(req.Args[0], duration)
	if err != nil {
		return "", false, err
	}

	return "OK!", false, nil
}

func (c *Command) actionDel(req dto.UserRequest) (string, bool, error) {
	if len(req.Args) < 1 {
		return "", false, errors.New("Usage: DEL key")
	}

	c.Storage.Del(req.Args[0])
	return "OK!", false, nil
}

func (c *Command) actionExit(req dto.UserRequest) (string, bool, error) {
	return "Bye!", true, nil
}
