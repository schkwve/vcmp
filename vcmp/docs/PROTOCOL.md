# VCMP Message Protocol

## Part 1: Header
The header is a part of every message. It will be used to identify the message and share some information

| Size (bytes)     | Name         | Description                                               |
|------------------|--------------|-----------------------------------------------------------|
| 5 - uint8_t[5]   | Magic        | Protocol Magic 4 bytes string with 0 terminator "VCMP"    |
| 1 - uint8_t      | Version      | Protocol version 1 for v1.0                               |
| 16 - uint64_t[2] | Message UUID | UUID for each message                                     |
| 8 - uint64_t     | Timestamp    | Unix Timestamp for the message                            |
| 32 - SHA256      | Sender Key   | SHA-256 hash of the sender's public key                   |
| 4 - uint32_t     | Payload Size | Length of the encrypted payload                           |