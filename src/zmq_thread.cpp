/**
 * @copyright Copyright (c) 2024 Philip Stegmaier. All rights reserved.
 * @license MIT license
 */
#include <chrono>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include <zmq.hpp>

#include "zmq_node.hpp"

using json = nlohmann::json;


int main(int argc, char* argv[]) {
    ZmqNodeWrapper zn;

    std::cout << "Starting ZMQ node" << std::endl;
    zn.start();
    
    zmq::socket_t socket(*zn.get_context(), zmq::socket_type::req);
    
    // Connects to wrapped ZMQ node
    socket.connect("inproc://" + zn.get_name());
    
    // Exchanges some messages over REQ/REP sockets
    zmq::message_t msg;
    json message;
    std::uint8_t* data;

    for (int m = 1; m <= 10; ++m) {
        message["messageNum"] = m;
        auto res = socket.send(zmq::message_t(json::to_bson(message)), zmq::send_flags::dontwait);
        std::cout << "Send result: " << res.value() << std::endl;
        res = socket.recv(msg, zmq::recv_flags::none);
        data = msg.data<std::uint8_t>();
        message = json::from_bson(std::vector<std::uint8_t>(data, data + msg.size()/sizeof(std::uint8_t)));
        std::cout << "Reply: " << message.dump() << std::endl;
        message.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    socket.close();
    
    // Sends stop signal to ZMQ node
    zn.stop();
    
    return 0;
}
