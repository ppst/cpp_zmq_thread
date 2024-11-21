/**
 * @copyright Copyright (c) 2024 Philip Stegmaier. All rights reserved.
 * @license MIT license
 */
#include "zmq_node.hpp"


void ZmqNode::run() {
    std::cout << "Node thread: " << std::this_thread::get_id() << std::endl;
    
    std::vector<zmq::poller_event<>> events(2);

    std::chrono::milliseconds timeout(10);
    int messageCount = 0;
    zmq::message_t msg;

    while (doRun) {
        const auto nin = poller->wait_all(events, timeout);
        if (nin < 1L) {
            continue;
        }
        std::cout << "Polling..." << std::endl;
        for (std::size_t e = 0; e < nin; ++e) {
            auto res = events[e].socket.recv(msg, zmq::recv_flags::none);
            if (events[e].socket.get(zmq::sockopt::routing_id).compare("controller") == 0) {
                std::cout << "Received stop signal" << std::endl;
                doRun = false;
                break;
            }
            std::cout << "Reception result: " << res.value() << std::endl;
            std::cout << msg.to_string() << std::endl;
            res = events[e].socket.send(zmq::buffer(std::string("Received ") + msg.to_string()), zmq::send_flags::dontwait);
        }
    }
}


void ZmqNodeWrapper::start() {
    std::cout << "Controller thread: " << std::this_thread::get_id() << std::endl;
    zthread = std::make_unique<std::future<void>>(std::async(std::launch::async, [this]() {
        ZmqNode node(this->name, this->zmqContext, zmq_node::CONTROL_SOCKET_NAME);
        node.run();
    }));
    // Waiting for node's ready message
    zmq::message_t msg;
    auto res = controlSock->recv(msg, zmq::recv_flags::none);
    std::cout << msg.to_string() << std::endl;
}