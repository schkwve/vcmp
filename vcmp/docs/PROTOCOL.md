# VCMP Message Protocol

## Part 1: Header

The header is included in every message to identify its type and provide essential protocol information.

### Header Structure

| Size (bytes)     | Name           | Description                                                        |
|------------------|----------------|--------------------------------------------------------------------|
| 5 (`uint8_t[5]`) | Magic          | Protocol identifier, a 4-byte string "VCMP" with a null terminator |
| 1 (`uint8_t`)    | Version        | Protocol version, set to 1 for version 1.0                         |
| 16 (`uint64_t[2]`) | Message UUID | Unique identifier for each message                                 |
| 8 (`uint64_t`)   | Timestamp      | Unix timestamp for the message                                     |
| 32 (`SHA256`)    | Sender Key     | SHA-256 hash of the sender's public key                            |
| 4 (`uint32_t`)   | Payload Size   | Length of the encrypted payload                                    |

### Packet Types
The protocol defines several packet types for different purposes:

- `VCMP_TYPE_VERIFY` (1): Used for verification packets
- `VCMP_TYPE_HS_INIT` (2): Used for initiating a handshake
- `VCMP_TYPE_HS_ACK` (3): Acknowledgment for handshake initialization
- `VCMP_TYPE_DATA` (4): Used for data transfer packets

---

This structure provides a consistent way to identify, verify, and handle messages within the VCMP protocol.
