# Constructing and running a ZeroMQ node in separate thread

A wrapper class *ZmqNodeWrapper* starts and stops a ZeroMQ node *ZmqNode* in a separate thread
using *std::async*.

The *main unit* uses the wrapper to construct the node and exchanges messages with it.

The ZeroMQ node has two sockets. It binds to a REP socket to receive messages and connects to the
PAIR socket bound by the controller to listen for a stop signal.

The main unit connects to the ZeroMQ node through a REQ socket.

## Design features

1. The ZeroMQ node runs in a separate thread.
2. Interaction with the ZeroMQ node happens only through the messaging interface.
3. The lifecycle of the node can be controlled by the wrapper class.
4. Destructor and stop methods gracefully shutdown connections and context.
