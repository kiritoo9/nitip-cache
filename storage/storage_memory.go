package storage

import (
	"errors"
	"nitipv5/dto"
	"sync"
	"time"
)

type StorageMemory struct {
	Mu   sync.RWMutex
	Data map[string]dto.Item
}

func NewStorageMemory() Storage {
	return &StorageMemory{
		Data: make(map[string]dto.Item),
	}
}

func (s *StorageMemory) Get(key string) (*dto.Item, error) {
	s.Mu.RLock()
	defer s.Mu.RUnlock()

	item, ok := s.Data[key]
	if !ok {
		return nil, errors.New("(nil)")
	}

	// check expired
	if item.ExpiredAt != nil {
		if time.Now().After(*item.ExpiredAt) {
			return nil, errors.New("(expired)")
		}
	}

	return &item, nil
}

func (s *StorageMemory) Set(key string, value string) {
	s.Mu.Lock()
	defer s.Mu.Unlock()

	s.Data[key] = dto.Item{
		Value:     value,
		ExpiredAt: nil,
	}
}

func (s *StorageMemory) Exp(key string, duration int) error {
	s.Mu.Lock()
	defer s.Mu.Unlock()

	item, ok := s.Data[key]
	if !ok {
		return errors.New("(nil)")
	}

	expiredAt := time.Now().Add(time.Duration(duration) * time.Second)
	s.Data[key] = dto.Item{
		Value:     item.Value,
		ExpiredAt: &expiredAt,
	}

	return nil
}

func (s *StorageMemory) Del(key string) {
	s.Mu.Lock()
	defer s.Mu.Unlock()

	delete(s.Data, key)
}
