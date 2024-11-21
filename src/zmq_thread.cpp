/**
 * @copyright Copyright (c) 2024 Philip Stegmaier. All rights reserved.
 * @license MIT license
 */
#include <chrono>
#include <iostream>
#include <thread>

#include <zmq.hpp>

#include "zmq_node.hpp"


int main(int argc, char* argv[]) {
    ZmqNodeWrapper zn;

    std::cout << "Starting ZMQ node" << std::endl;
    zn.start();
    
    zmq::socket_t socket(*zn.get_context(), zmq::socket_type::req);
    
    // Connects to wrapped ZMQ node
    socket.connect("inproc://" + zn.get_name());
    
    // Exchanges some messages over REQ/REP sockets
    zmq::message_t msg;
    for (int m = 1; m <= 10; ++m) {
        auto res = socket.send(zmq::buffer(std::string("Message ") + std::to_string(m)), zmq::send_flags::dontwait);
        std::cout << "Send result: " << res.value() << std::endl;
        res = socket.recv(msg, zmq::recv_flags::none);
        std::cout << "Reply: " << msg.to_string() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    socket.close();
    
    // Sends stop signal to ZMQ node
    zn.stop();
    
    return 0;
}
