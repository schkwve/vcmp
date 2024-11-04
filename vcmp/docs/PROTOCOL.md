# VCMP Message Protocol

The VCMP Message Protocol is designed for secure peer-to-peer communication over Tor, used in the VCMP (Very Cool Message Platform). This protocol provides structured message types for verification, handshake initiation, handshake acknowledgment, and data transmission.

## Protocol Overview

All VCMP messages begin with a common header structure, allowing clients to identify message type, version, and other metadata. The protocol supports different packet types to handle initial verification, handshake, and data transmission.

---

## Part 1: Common Header

The header is included in every message type and contains general information required for message identification and handling.

| Field           | Size (bytes) | Type       | Description                                          |
|-----------------|--------------|------------|------------------------------------------------------|
| `magic`         | 5            | `uint8_t`  | Protocol identifier (`"VCMP\0"`)                     |
| `version`       | 1            | `uint8_t`  | Protocol version (currently `1`)                     |
| `type`          | 1            | `uint8_t`  | Packet type identifier                               |

## Part 2: Packet Types

### 1. Verification Packet (`VCMP_TYPE_VERIFY`)

Used for initial verification between peers. Contains only the common header.

| Field           | Size (bytes) | Type             | Description                                      |
|-----------------|--------------|------------------|--------------------------------------------------|
| `header`        | 7            | `vcmp_header_t`  | Common header, with `type = VCMP_TYPE_VERIFY`    |

### 2. Handshake Packets

#### Handshake Initialization (`VCMP_TYPE_HS_INIT`)

Initiates the handshake by sending the sender’s RSA public key to the peer.

| Field           | Size (bytes)   | Type             | Description                                       |
|-----------------|----------------|------------------|---------------------------------------------------|
| `header`        | 7              | `vcmp_header_t`  | Common header, with `type = VCMP_TYPE_HS_INIT`    |
| `keylen`        | 4              | `uint32_t`       | Length of the public key                          |
| `pubkey`        | `keylen`       | `uint8_t[]`      | RSA public key in DER format                      |

#### Handshake Acknowledgment (`VCMP_TYPE_HS_ACK`)

Acknowledges receipt of the handshake initialization packet.

| Field           | Size (bytes) | Type             | Description                                        |
|-----------------|--------------|------------------|----------------------------------------------------|
| `header`        | 7            | `vcmp_header_t`  | Common header, with `type = VCMP_TYPE_HS_ACK`      |

### 3. Data Packet (`VCMP_TYPE_DATA`)

Used to transmit encrypted messages between peers. Includes a unique message identifier, a timestamp, and the encrypted payload.

| Field           | Size (bytes)   | Type             | Description                                        |
|-----------------|----------------|------------------|----------------------------------------------------|
| `header`        | 7              | `vcmp_header_t`  | Common header, with `type = VCMP_TYPE_DATA`        |
| `message_uuid`  | 16             | `uint64_t[2]`    | Unique identifier for the message                  |
| `timestamp`     | 8              | `uint64_t`       | Unix timestamp for the message                     |
| `len`           | 4              | `uint32_t`       | Length of the encrypted payload                    |
| `payload`       | `len`          | `uint8_t[]`      | Encrypted message content                          |
