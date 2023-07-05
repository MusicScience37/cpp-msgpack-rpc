Image of Usage
=====================

Client
-------------------

.. code-block:: cpp

    // Create a client.
    const std::string host = "localhost";
    const std::uint16_t port = 12345;
    Client client(host, port);

    // Call a method named "add" with parameter 1 and 2.
    const int result = client.call<int>("add", 1, 2)

    // Asynchronously call.
    // Parameters are serialized in this function call to prevent use of parameters after destruction.
    client.async_call<int>([] (int result) { print(result); } , "add", 1, 2);

    // Send notification to a method named "publish" with parameter "abc".
    client.notify("publish", "abc");

Server
-------------------

.. code-block:: cpp

    ServerBuilder builder;

    // Set an address to listen to.
    const std::string host = "localhost";
    const std::uint16_t port = 12345;
    builder.listen_to_tcp(host, port);

    // Add some methods.
    builder.add_method<int(int, int)>("add", [] (int a, int b) { return a + b; });
    builder.add_method<void(std::string)>("publish", [] (const std::string& str) { print(str); });

    // Create a server.
    Server server = builder.build();

    // Start the server.
    server.start();

    // Stop the server.
    server.stop();
