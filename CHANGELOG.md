# Changelog

Semua perubahan signifikan di project ini akan didokumentasikan di file ini.

Format mengikuti [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [0.1.0] - 2026-06-23

### Added
- **Multi-Tenancy** — Setiap client wajib authenticate dengan token sebelum bisa akses data. Data antar tenant terisolasi via key prefix `tenant_id:`.
- **Token Generator** — `./nitip gen-token <tenant_id> [--ttl <detik>` untuk generate token baru. Token disimpan di `nitip.auth`.
- **Token TTL** — Token bisa expires otomatis setelah N detik.
- **Token Revocation** — `./nitip revoke-token <tenant_id>` untuk hapus semua token milik satu tenant.
- **Token List** — `./nitip list-tokens` untuk lihat semua token aktif + waktu expiry.
- **WHOAMI / WHO Command** — Client bisa cek tenant ID yang sedang aktif.
- **Structured Logging** — Log dengan format `[timestamp] [LEVEL] message`, Level: INFO / WARN / ERROR.
- **INFO Command** — Command baru yang ngembaliin version, uptime (detik), dan total key count.

### Changed
- Command delimiter diubah dari `\n` ke `\r\n` (Redis-style) untuk kompatibilitas network client.
- Server listen queue dari 1 ke `SOMAXCONN` (persiapan concurrent clients).
- Buffer read diperbesar dari 1024 ke 4096 bytes.
- Nilai multi-word untuk SET sekarang menangkap semua token setelah key, tidak cuma token kedua.
- `main.cpp` sekarang handle mode server + CLI tools dalam satu binary.
- Code formatting dirapikan (single-line blocks, konsistent style).
- `engine.cpp` now includes `engine.h` explicitly, making includes cleaner.

### Fixed
- `setsockopt SO_REUSEADDR` sekarang enabled, mencegah "Address already in use" saat restart.
- Typo `s_add` → `s_addr` di `server.cpp` yang menyebabkan build error.
- Auth manager tidak crash jika file `nitip.auth` tidak ada (fallback ke warn message).

### Security
- Semua command (SET/GET/DEL/EXPIRE) butuh authentication — client tanpa AUTH akan ditolak dengan `-ERR not authenticated`.
- Invalid token akan ditolak dengan `-ERR invalid token`.
- Key isolation 100% — tidak ada cara satu tenant bisa akses data tenant lain.

---

## [0.0.11] - Sebelum

Versi sebelumnya tidak memiliki changelog. Rilis awal dengan fitur:
- SET, GET, DEL, EXPIRE
- Persistence (nitip.db)
- TCP Server
- CLI mode

---

*Changelog dimulai dari versi 0.1.0 saat fitur multi-tenancy ditambahkan.*
