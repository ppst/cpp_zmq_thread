/**
 * @copyright Copyright (c) 2024 Philip Stegmaier. All rights reserved.
 * @license MIT license
 */
#pragma once

#include <chrono>
#include <future>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <zmq.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


namespace zmq_node {

const std::string DEFAULT_NODE_NAME = "zmq_node";
const std::string CONTROL_SOCKET_NAME = "controller";

}


/** ZMQ node to be run in a separate thread.
 * 
 */
class ZmqNode {
public:
    ZmqNode() = delete;

    ZmqNode(const std::string& name, std::shared_ptr<zmq::context_t> zmqContext, const std::string& controlName):
                name{name},
                zmqContext{zmqContext},
                controlSock{std::make_shared<zmq::socket_t>(*zmqContext, zmq::socket_type::pair)},
                taskSock{std::make_shared<zmq::socket_t>(*zmqContext, zmq::socket_type::rep)},
                poller{std::make_shared<zmq::poller_t<>>()},
                doRun{true} {
        controlSock->connect("inproc://" + zmq_node::CONTROL_SOCKET_NAME);
        controlSock->set(zmq::sockopt::routing_id, "controller");
        taskSock->bind("inproc://" + name);
        taskSock->set(zmq::sockopt::routing_id, "task");
        poller->add(*controlSock, zmq::event_flags::pollin);
        poller->add(*taskSock, zmq::event_flags::pollin);
        controlSock->send(zmq::str_buffer("Ready"), zmq::send_flags::dontwait);
    }

    ~ZmqNode() {
        std::cout << "Closing bound task socket" << std::endl;
        taskSock->close();
        std::cout << "Closing connected control socket" << std::endl;
        controlSock->close();
    }

    void run();

private:
    std::string name;
    std::shared_ptr<zmq::context_t> zmqContext;
    std::shared_ptr<zmq::socket_t> taskSock;
    std::shared_ptr<zmq::socket_t> controlSock;
    std::shared_ptr<zmq::poller_t<>> poller;
    bool doRun;
};


/** Wrapper class that launches ZMQ node in separate thread and provides a stop function.
 * 
 */
class ZmqNodeWrapper {
public:
    ZmqNodeWrapper(): name{zmq_node::DEFAULT_NODE_NAME}, 
                      zmqContext{std::make_shared<zmq::context_t>(zmq::context_t(0))},
                      controlSock(std::make_shared<zmq::socket_t>(*zmqContext, zmq::socket_type::pair)),
                      zthread(nullptr) {
        controlSock->bind("inproc://" + zmq_node::CONTROL_SOCKET_NAME);
    }

    ~ZmqNodeWrapper() {
        std::cout << "Closing bound control socket" << std::endl;
        controlSock->close();
        std::cout << "Shutting down owned context" << std::endl;
        zmqContext->shutdown();
        zmqContext->close();
        std::cout << "Done" << std::endl;
    }

    std::string get_name() const { return name; }

    std::shared_ptr<zmq::context_t> get_context() const { return zmqContext; }

    void start();

    void stop() const {
        auto res = controlSock->send(zmq::str_buffer(""), zmq::send_flags::dontwait);
        zthread->get();
    }

private:
    std::string name;
    std::shared_ptr<zmq::context_t> zmqContext;
    std::shared_ptr<zmq::socket_t> controlSock;
    std::unique_ptr<std::future<void>> zthread;
};