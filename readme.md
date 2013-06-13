# libhttplightcontrol

This provides a protocol for which an Arduino can run a server application and clients can connect and send messages.

Communication is wireless through XBee radios, but the server can be ethernet or wirelessly connected to the local network.

There are utilities that can be used by client and server applications alike to process messages and commands in the system.

Design Goals:
- Provide a platform agnostic (web-based) interface for controlling relays and LED strips.
- N number of clients connected to the server, must be able to be dynamically added




Road Map:
- Clients should ping the server every so often to maintain "online" state -- when the power dies, sometimes the clients can boot up faster than the server and they don't get registered properly. This also allows clients to be taken offline and online again and have the server reflect only the "online" clients. This alleviates restarting the server if a client is taken offline or otherwise lost (due to communication problems, etc).

