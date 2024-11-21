# Constructing and running a ZeroMQ node in separate thread

A wrapper class [ZmqNodeWrapper](https://github.com/ppst/cpp_zmq_thread/blob/d462c6ebf9c36f1467d857ea86ec9bd1269afed0/include/zmq_node.hpp#L71) starts and stops a ZeroMQ node [ZmqNode](https://github.com/ppst/cpp_zmq_thread/blob/d462c6ebf9c36f1467d857ea86ec9bd1269afed0/include/zmq_node.hpp#L29) in a separate thread
using [std::async](https://github.com/ppst/cpp_zmq_thread/blob/d462c6ebf9c36f1467d857ea86ec9bd1269afed0/src/zmq_node.cpp#L40).

The [main unit](https://github.com/ppst/cpp_zmq_thread/blob/d462c6ebf9c36f1467d857ea86ec9bd1269afed0/src/zmq_thread.cpp#L14) uses the wrapper to construct the node and exchanges messages with it.

The ZeroMQ node has two sockets. It binds to a REP socket to receive messages and connects to the
PAIR socket bound by the controller to listen for a stop signal.

The main unit connects to the ZeroMQ node through a REQ socket.

## Design features

1. The ZeroMQ node runs in a separate thread.
2. Interaction with the ZeroMQ node happens only through the messaging interface.
3. The lifecycle of the node can be controlled by the wrapper class.
4. Destructor and stop methods gracefully shutdown connections and context.
