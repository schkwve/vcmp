# Chat Room Documentation

## Overview
Each chat room includes a Name, Description, and a unique UUID. The Name and Description are for display purposes only and do not impact the protocol functionality. Chat rooms have no strict member limit, though maximum capacity depends on the host’s resources. As VCMP operates on a Peer-to-Peer (P2P) model, there is no centralized server or dedicated host for rooms; instead, the room’s creator serves as the host.

## Member List Structure
The member list is implemented as a linked list, allowing dynamic growth as members join. Usernames are limited to 256 characters, room names to 64 characters, and descriptions to 256 characters, though these limits can be adjusted in future versions.

## Example Implementation (C)

**Member List Structure**:
```c
struct member {
    uint64_t uuid[2];
    char username[32];
    struct member* next;
};

```
**Room Structure**:
```c
struct room {
    uint64_t uuid[2];
    char room_name[64];
    char room_description[256];
    struct member *head;
}
```

<!--- TODO for me (Raphael): Add a USER.md documentation for user specific shit lmao --->