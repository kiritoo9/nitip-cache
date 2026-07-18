package storage

import "nitipv5/dto"

type Storage interface {
	Get(key string) (*dto.Item, error)
	Set(key string, value string)
	Exp(key string, duration int) error
	Del(key string)
}
