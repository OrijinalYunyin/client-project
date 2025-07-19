// client.cpp
// This is a Subscriber (SUB) for our chat application in C++.
// It connects to the server's PUB socket to receive messages
// and has a PUSH socket to send messages to the server's PULL socket.

#include <zmq.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

// This function will run in a separate thread to receive messages
void receive_messages(zmq::socket_t& subscriber, std::atomic<bool>& running) {
    while (running) {
        try {
            zmq::message_t received_msg;
            // Use non-blocking receive so the loop can check the `running` flag
            if (subscriber.recv(received_msg, zmq::recv_flags::dontwait)) {
                std::cout << "\r" << received_msg.to_string() << std::endl << "> " << std::flush;
            } else {
                // If no message, sleep for a short while to prevent busy-waiting
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        } catch (const zmq::error_t& e) {
            // If the context is terminated, the recv call will throw an exception
            if (e.num() == ETERM) {
                break; // Exit loop cleanly
            }
            std::cerr << "Error receiving message: " << e.what() << std::endl;
            break;
        }
    }
}

int main() {
    zmq::context_t context(1);
    const std::string server_address = "tcp://51.195.118.2";

    // --- Subscriber Socket ---
    // Connects to the server's PUB socket to receive broadcasts
    zmq::socket_t subscriber(context, ZMQ_SUB);
    const std::string pub_port = "5556";
    subscriber.connect(server_address + ":" + pub_port);
    // Subscribe to all topics (empty string)
    subscriber.set(zmq::sockopt::subscribe, "");
    std::cout << "Connected to server publisher at " << server_address << ":" << pub_port << std::endl;


    // --- Pusher Socket ---
    // Connects to the server's PULL socket to send messages
    zmq::socket_t pusher(context, ZMQ_PUSH);
    const std::string pull_port = "5557";
    pusher.connect(server_address + ":" + pull_port);
    std::cout << "Connected to server receiver at " << server_address << ":" << pull_port << std::endl;


    // --- User Interaction ---
    std::string username;
    std::cout << "Enter your username: ";
    std::getline(std::cin, username);
    std::cout << "Welcome, " << username << "! Type your messages and press Enter to send." << std::endl;
    std::cout << "Type 'exit' to quit." << std::endl;

    // --- Start receiver thread ---
    std::atomic<bool> running(true);
    std::thread receiver_thread(receive_messages, std::ref(subscriber), std::ref(running));

    // --- Main loop to send messages ---
    std::cout << "> " << std::flush;
    std::string message_text;
    while (std::getline(std::cin, message_text)) {
        if (message_text == "exit") {
            break;
        }

        std::string full_message = "[" + username + "]: " + message_text;
        pusher.send(zmq::buffer(full_message), zmq::send_flags::none);
        std::cout << "> " << std::flush;
    }

    // --- Cleanup ---
    std::cout << "\nDisconnecting..." << std::endl;
    running = false; // Signal the receiver thread to stop
    receiver_thread.join(); // Wait for the receiver thread to finish

    // Sockets and context are closed automatically by their destructors.
    return 0;
}
