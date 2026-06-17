# Nitip

Nitip adalah Redis clone dan alternatif ringan untuk Redis yang dibangun menggunakan C++. Menyediakan penyimpanan data key-value di memory dengan fokus pada performa tinggi, konsumsi resource yang efisien, dan kemudahan integrasi.

Cocok digunakan untuk caching, session storage, rate limiting, queue, dan berbagai kebutuhan akses data berlatensi rendah tanpa memerlukan deployment Redis yang kompleks.

## Fitur

* Penyimpanan key-value di memory
* Persistence (Save & Load)
* Key Expiration (TTL)
* TCP Server
* Arsitektur modular
* Lightweight deployment

## Contoh Penggunaan

```text
SET name Jon
OK

GET name
Jon

DEL name
OK

GET name
(nil)
```

## Build

```bash
mkdir build
cd build

cmake ..
make
```

## Menjalankan Server

```bash
./nitip
```

Server akan berjalan pada:

```text
localhost:6379
```

## Arsitektur

```text
Client
  │
  ▼
TCP Server
  │
  ▼
Nitip Engine
  │
  ├── In-Memory Storage
  └── Persistence Layer
```

## Roadmap

### Core

* [x] SET
* [x] GET
* [x] DEL
* [x] Persistence (Save/Load)
* [x] Key Expiration (TTL)
* [x] TCP Server

### Networking

* [ ] Multiple Clients
* [ ] Connection Pooling
* [ ] RESP Protocol Compatibility

### Scalability

* [ ] Pub/Sub
* [ ] Replication
* [ ] Clustering

### Observability

* [ ] Logging
* [ ] Metrics
* [ ] Monitoring Endpoint

## Final Goal

Menjadi alternatif Redis yang ringan dan mudah dideploy untuk kebutuhan caching dan penyimpanan data berkecepatan tinggi, dengan implementasi modern berbasis C++.

## Version
0.0.11

## Author
kiritoo9

---

*"Kalau belum perlu database besar, nitip dulu."*
