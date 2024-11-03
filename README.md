# VCMP - Very Cool Message Platform

**VCMP** stands for **Very Cool Message Platform**, a peer-to-peer chat platform with privacy at its core. VCMP emphasizes Tor-based anonymity and secure, RSA-encrypted communication, but it can integrate with alternative privacy solutions if needed.

## About the Project

VCMP enables secure, decentralized communication through:
- **End-to-End Encryption**: RSA encryption secures messages between peers.
- **Mutual Authentication**: Peers verify each other via a multi-step handshake before communication begins.
- **Tor-based Anonymity**: Tor hostnames can serve as public addresses, adding a layer of privacy by obscuring connection details.

### Authentication and Connection Process

1. **Initial Packet**: A peer initiates contact by sending an initial packet.
2. **Verification and Key Exchange**: Upon acknowledgment, the receiver shares its RSA public key. The sender also sets its own public key.
3. **Authentication Test**: The sender sends an encrypted dummy packet, which the receiver decrypts and returns to confirm authenticity.
4. **Secure Communication**: Once authenticated, encrypted communication begins.

### Technical Highlights

- **Backend**: Built entirely in C for optimized performance.
- **Frontend Compatibility**: Any language that supports WebSocket communication can be used for frontend development, enabling flexibility across different platforms.
  
## Project Architecture

Each client functions as an independent node, establishing secure and private connections with other nodes as desired. VCMP's architecture inherently supports decentralized, peer-to-peer communication with privacy-first principles.

## Getting Started

### Prerequisites

Ensure the following libraries are installed:

- [libevent](https://github.com/libevent/libevent)
- [ws (WebSocket library)](https://github.com/splexas/ws)
- [OpenSSL](https://github.com/openssl/openssl)
- libuuid (package `util-linux` in many Linux distributions)

### Building & Running

```bash
git clone https://github.com/splexas/vcmp.git
cd vcmp/vcmp
mkdir build
cd build
cmake ..
cmake --build .
./vcmp_client
```
# License
This project is licensed under the MIT License. For more details, please refer to the [LICENSE](./LICENSE) file.