# Nitip - Cache Engine

Nitip adalah Redis clone dan alternatif ringan untuk Redis yang dibangun menggunakan C++. Menyediakan penyimpanan data key-value di memory dengan fokus pada performa tinggi, konsumsi resource yang efisien, dan kemudahan integrasi.

Cocok digunakan untuk caching, session storage, rate limiting, queue, dan berbagai kebutuhan akses data berlatensi rendah tanpa memerlukan deployment Redis yang kompleks.

## Fitur

- Penyimpanan key-value di memory
- Persistence (Save & Load)
- Key Expiration (TTL)
- TCP Server
- Logging dengan timestamp
- INFO command (version, uptime, key count)
- Multi-tenancy dengan token-based auth
- Token generator dengan TTL support
- Token revocation
- WHOAMI command

## Build

```bash
mkdir build
cd build
cmake ..
make
```

## Menjalankan Server

```bash
# Default port 6379
./nitip

# Custom port
./nitip 8080
```

## Multi-Tenancy

### Generate Token

```bash
# Token tanpa expiry (selamanya)
./nitip gen-token <tenant_id>

# Token dengan TTL (detik)
./nitip gen-token <tenant_id> --ttl 3600

# Contoh
./nitip gen-token my-app
./nitip gen-token other-app --ttl 86400  # 24 jam
```

Token disimpan di file `nitip.auth` secara otomatis.

### List Token

```bash
./nitip list-tokens
```

Output:
```
Active tokens:
  Tenant: my-app
  Token:  nitip_abc123...
  Expires: never

  Tenant: other-app
  Token:  nitip_def456...
  Expires: 1782250000 (86400s remaining)

Total: 2 active token(s)
```

### Revoke Token

Hapus semua token untuk tenant tertentu:

```bash
./nitip revoke-token <tenant_id>

# Contoh
./nitip revoke-token my-app
```

## Client Commands

Setelah terhubung ke server, client harus authenticate terlebih dahulu sebelum bisa menggunakan command lain:

### AUTH

```text
AUTH <token>
+OK

# Jika token invalid
AUTH wrong-token
-ERR invalid token
```

### SET

```text
AUTH <token>
+OK

SET name Jon
+OK
```

### GET

```text
GET name
Jon

GET nonexistent
(nil)
```

### DEL

```text
DEL name
+OK
```

### EXPIRE

```text
SET session abc123
+OK

EXPIRE session 3600
+OK
```

### INFO

```text
INFO
Version: 0.0.11
Uptime: 1234s
Keys: 5
```

### WHOAMI / WHO

```text
WHOAMI
my-app

WHO
my-app
```

## Contoh Penggunaan

### Client A (tenant: alpha)

```bash
nc localhost 6379

auth nitip_abc123
+OK

set name Jon
+OK

get name
Jon
```

### Client B (tenant: beta)

```bash
nc localhost 6379

auth nitip_def456
+OK

set name Jane
+OK

get name
Jane
```

Data terisolasi — Client A tidak bisa melihat data Client B.

## Logging

Server menampilkan log dengan format rapi:

```
[2026-06-23 15:00:28] [INFO] Loaded 2 auth token(s)
[2026-06-23 15:00:28] [INFO] Nitip server started on port 6379
[2026-06-23 15:00:30] [INFO] Client connected from 127.0.0.1
[2026-06-23 15:00:30] [INFO] Tenant 'alpha' authenticated from 4
[2026-06-23 15:00:30] [INFO] SET key=alpha:name value=Jon
[2026-06-23 15:00:30] [INFO] GET key=alpha:name result=Jon
[2026-06-23 15:00:30] [INFO] Client disconnected
```

## Arsitektur

```text
Client
  │
  ├── AUTH (token → tenant)
  │
  ▼
TCP Server
  │
  ▼
Nitip Engine (prefix key: tenant:)
  │
  ├── In-Memory Storage
  └── Persistence Layer (nitip.db)
```

## Docker

### Build & Run

```bash
docker compose up -d
```

Image size: ~2.3MB (multi-stage build, scratch base).

### Token Management via Docker

Container berbasis `scratch` — tidak ada shell. Semua CLI commands harus pakai `-w /data` agar CWD sesuai lokasi `nitip.auth`.

```bash
# Generate token (tanpa expiry)
docker exec -w /data nitip /nitip gen-token <tenant_id>

# Generate token dengan TTL (detik)
docker exec -w /data nitip /nitip gen-token <tenant_id> --ttl 86400

# List semua token aktif
docker exec -w /data nitip /nitip list-tokens

# Revoke token tenant
docker exec -w /data nitip /nitip revoke-token <tenant_id>
```

### Data Persistence

`nitip.auth` dan `nitip.db` disimpan di Docker named volume `nitip-data` (mount ke `/data` di dalam container). Data tidak hilang saat container restart.

### Ubah Port

Edit `docker-compose.yml`:

```yaml
ports:
  - "6380:6379"   # host:container
```

## File Structure

```
.
├── src/
│   ├── main.cpp        Entry point + CLI tools
│   ├── engine.h/cpp    Core engine (SET/GET/DEL/EXPIRE)
│   ├── server.h/cpp    TCP server + client handler
│   ├── auth.h          AuthManager (token/tenant)
│   └── logger.h        Logging
├── build/
├── nitip.auth          Token store (generated)
├── nitip.db            Persistence file (auto-save)
├── CMakeLists.txt
└── README.md
```

## Roadmap

### Core
- [x] SET
- [x] GET
- [x] DEL
- [x] Persistence (Save/Load)
- [x] Key Expiration (TTL)
- [x] TCP Server
- [x] Logging
- [x] INFO command
- [x] Multi-tenancy (AUTH, key isolation)

### Networking
- [ ] Multiple Clients (concurrent)
- [ ] Connection Pooling
- [ ] RESP Protocol Compatibility

### Scalability
- [ ] Pub/Sub
- [ ] Replication
- [ ] Clustering

### Observability
- [ ] Metrics (Prometheus endpoint)
- [ ] Monitoring Dashboard

## Final Goal

Menjadi alternatif Redis yang ringan dan mudah dideploy untuk kebutuhan caching dan penyimpanan data berkecepatan tinggi, dengan implementasi modern berbasis C++.

## Version

0.1.0

## Author

kiritoo9

---

*"Kalau belum perlu database besar, nitip dulu."*
