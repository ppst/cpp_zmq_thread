/**
 * @copyright Copyright (c) 2024 Philip Stegmaier. All rights reserved.
 * @license MIT license
 */
#include <chrono>
#include <iostream>
#include <thread>

#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <zmq.hpp>

#include "zmq_node.hpp"

using json = nlohmann::json;
using json_validator = nlohmann::json_schema::json_validator;


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

    const json replySchema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "Reply",
        "properties": {
            "count": {
                "$ref": "#/definitions/count"
            },
            "messageNum": {
                "$ref": "#/definitions/messageNum"
            }
        },
        "definitions": {
            "count": {
                "type": "integer",
                "minimum": 1
            },
            "messageNum": {
                "type": "integer",
                "minimum": 1
            }
        }
    })"_json;

    json_validator validator{replySchema};

    for (int m = 1; m <= 10; ++m) {
        message["messageNum"] = m;
        auto res = socket.send(zmq::message_t(json::to_bson(message)), zmq::send_flags::dontwait);
        std::cout << "Send result: " << res.value() << std::endl;
        res = socket.recv(msg, zmq::recv_flags::none);
        data = msg.data<std::uint8_t>();
        message = json::from_bson(std::vector<std::uint8_t>(data, data + msg.size()/sizeof(std::uint8_t)));
        try {
            validator.validate(message);
            std::cout << "Reply: " << message.dump() << std::endl;
        } catch (const std::exception &e) {
            std::cerr << "Message validation failed: " << e.what() << "\n";
        }
        message.clear();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    
    socket.close();
    
    // Sends stop signal to ZMQ node
    zn.stop();
    
    return 0;
}
