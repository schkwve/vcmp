# Events from Backend to Frontend

## Event: user_join
The `user_join` event is triggered when a user joins a room.
Usage: 
```json
{
    "event": "user_join",
    "username": "Example",
    "room": "fc3667cb-fe5b-4685-8ef7-f780deaa322e"
}
```
This tells the frontend that the user `Example` joined the room `fc3667cb-fe5b-4685-8ef7-f780deaa322e`.

## Event: user_leave
The `user_leave` event is triggered when a user leaves the room or gets disconnected.
Usage:
```json
{
    "event": "user_leave",
    "username": "Example",
    "room": "fc3667cb-fe5b-4685-8ef7-f780deaa322e"
}
```
This tells to frontend that the user `Example` left the room `fc3667cb-fe5b-4685-8ef7-f780deaa322e`.

## Event: user_message
The `user_message` event is triggered when a user has send a message to the other Nodes. The backend will decrypt it and send it to the frontend.
Example:
```json
{
    "event": "user_message",
    "username": "Example",
    "timestamp": 1730736486,
    "message": "Hello, world!",
    "room": "fc3667cb-fe5b-4685-8ef7-f780deaa322e"
}
```
This tells the frontend that the user `Example` send the message `Hello, world!` at the UNIX timestamp `1730736486`

## Event: user_invite
The `user_invite` event is triggered when the `sender` is inviting the `target` to join the room with the UUID in `room`
Example:
```json
{
    "event": "user_invite",
    "sender": "kevinalavik",
    "room": "fc3667cb-fe5b-4685-8ef7-f780deaa322e"
}
```
In this case the user `kevinalavik` invites the user `splexas` to join the room with the UUID `fc3667cb-fe5b-4685-8ef7-f780deaa322e`

# Events from Backend to Frontend

## Event: user_invite_response
The `user_invite_response` is **always** send after a user send another user an invite. The are 3 different responses to an invite: `decline`, `accept` and `ignore`. The ignore event can only be triggered by not responding in 120 seconds after the invite was recieved
Example: 
```json
{
    "event": "user_invite_response",
    "response": "accept",
}
```
This tells the backend that the user from the previous `user_invite` event has accepted the invite. The user will send a `user_join` event after this