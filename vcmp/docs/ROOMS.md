# Chat Rooms Documentation

## Basic Structure
Chat rooms have a Name, Description, and a UUID. The Name and Description are only used for Display and have no functions in the protocol. A chat room can have an unlimited members, though the maximum size is defined based on the resources of the host. Since VCMP is based on Peer2Peer there is not server / room actually hosting it the creator of the room is hosting the room. The member list can be a linked list. Below is an example in the C programming language that implements a room. These rooms can be created, destoryed and edited. The usernames have a limit of 256 characters, and the room name has a limit of 64 characters, and the room description can be 256 characters long. These limitations can be changed in version 2.

### C example
The member list struct:
```c
struct member {
    uint64_t uuid[2];
    char username[32];
    struct member* next;
};
```
The room struct:
```c
struct room {
    uint64_t uuid[2];
    char room_name[64];
    char room_description[256];
    struct member *head;
}
```

<!--- TODO for me (Raphael): Add a USER.md documentation for user specific shit lmao --->