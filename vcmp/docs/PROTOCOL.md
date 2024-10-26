# VCMP Message Protocol

## Part 1: Header
The header is a part of every message. It will be used to identify the message and share some information

| Size (bytes) | Name         | Description                                               |
|--------------|--------------|-----------------------------------------------------------|
| 8 - uint64_t | Magic        | Protocol Magic 7 bytes string with 0 terminator "VCMPP 1" |
| 1 - uint8_t  | Version      | Protocol version 1 for v1.0                               |
| 8 - uint64_t | Messsage UID | UUID for each message                                     |
| 8 - uint64_t | Timestamp    | Unix Timestamp for the message                            |
| 32 - SHA256  | Sender UID   | SHA-256 hash of the sender's public key                   |
| 32 - SHA256  | Reciever UID | SHA-256 hash of the reciever's public key                 |
| 4 - uint32_t | Payload Size | Lenght of the encrypted payload                           |