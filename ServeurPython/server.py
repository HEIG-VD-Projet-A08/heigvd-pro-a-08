#!/usr/bin/python3

import socket
from xml.dom.minidom import parse, parseString
import globals

SERVER_PORT = 9001
IP = "127.0.0.1"

# Protocol
PROTOCOL_HELLO_REC = "Hello Server\n"
PROTOCOL_HELLO_SEND = "Hello Client\n"
PROTOCOL_START_REC = "START\n"
PROTOCOL_STOP_REC = "STOP\n"
PROTOCOL_STOP_SEND = "STOP experiment\n"
PROTOCOL_REQUEST_STOP_REC = "STOP -R\n"
PROTOCOL_REQUEST_STOP_SEND = "STOP experiment,intermediate result in Result.xml\n"
PROTOCOL_CLIENT_TERMINATE_REC = "BYE\n"
PROTOCOL_UNKNOWN_COMMAND_SEND = "ERROR Bad Message\n"


"""
@brief Deserializes XML data received from the client
"""
def deserialize_xml():
    data_source = open("param.xml")
    dom = parse(data_source)
    nb_words = dom.getElementsByTagName("Nb_words")[0].firstChild.data
    nb_char_max = dom.getElementsByTagName("Nb_char_Max")[0].firstChild.data
    nb_char_min = dom.getElementsByTagName("Nb_char_Min")[0].firstChild.data
    nb_iter = dom.getElementsByTagName("Nb_iter")[0].firstChild.data

    return [int(nb_char_min), int(nb_char_max)], int(nb_words), int(nb_iter)


"""
@brief Initializes the server at first launch
"""
def server_init():
    globals.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_name = socket.gethostname()
    print("starting up on", server_name, "port", SERVER_PORT)
    globals.server_socket.bind((IP, SERVER_PORT))
    globals.server_socket.listen(1)  # Allow only one connection request before refusing
    globals.first_serv_init = False


"""
@brief Opens a channel where the client can send the necessary parameters for the evoltionnary algorithm to work
@note Closes the client connection and wait for a new one to be made as per the client's implementation
@:return the word size, the number of words, the number of iterations, and the client socket needed by the evolutionnaryAlgorithm
"""
def client_data_channel():
    if globals.first_serv_init is True:
        server_init()

    print("waiting for connections")
    (client_socket, client_address) = globals.server_socket.accept()

    start_received = False
    xml_received = False
    should_listen = True

    try:
        data = client_socket.makefile()
        param_xml = open("param.xml", 'w')
        while should_listen:
            for line in data:
                if line is not None:
                    print("received:", line)
                    if line == PROTOCOL_HELLO_REC:
                        client_socket.send(PROTOCOL_HELLO_SEND.encode('utf-8'))
                    elif line == PROTOCOL_START_REC:
                        start_received = True
                    elif start_received and line.lstrip().startswith("<"):
                        param_xml.write(line)
                        if line == "</Options>\n":
                            print("Finished receiving options data")
                            param_xml.close()
                            xml_received = True
                            should_listen = False
                            break
                    elif line == PROTOCOL_CLIENT_TERMINATE_REC:
                        should_listen = False
                        break
                    else:
                        print("Unknown setup received")
                        client_socket.send(PROTOCOL_UNKNOWN_COMMAND_SEND.encode('utf-8'))
                        should_listen = False
                        break
    finally:
        print("closing data connection")
        if not xml_received:
            print("Options not received or badly formatted")
            word_size, nb_words, nb_iter = [int(30), int(49)], 10, 0
        else:
            word_size, nb_words, nb_iter = deserialize_xml()
        # necessary because of how the client works
        client_socket.close()
        (client_socket, client_address) = globals.server_socket.accept()

    return word_size, nb_words, nb_iter, client_socket


"""
@brief Opens a communication channel between the client and the server during the evolution
@:param cs, the client_socket opened at the end of client_data_channel(), passed to/from the evolutionary algorithm
"""
def client_commands_thread(cs):
    print("opening command thread")
    client_socket = cs

    try:
        data = client_socket.makefile()
        while globals.shouldListen:
            for line in data:
                if line is not None:
                    print("received:", line)
                    if line == PROTOCOL_STOP_REC:
                        globals.stopReceived = True
                        client_socket.send(PROTOCOL_STOP_SEND.encode('utf-8'))
                        break
                    elif line == PROTOCOL_REQUEST_STOP_REC:
                        # Finish iteration + wait for PROTOCOL_CLIENT_TERMINATE_REC
                        globals.stopRequested = True
                        client_socket.send(PROTOCOL_REQUEST_STOP_SEND.encode('utf-8'))
                        break
                    elif line == PROTOCOL_CLIENT_TERMINATE_REC:
                        globals.stopReceived = True
                        globals.shouldListen = False
                        break
                    else:
                        print("Unknown command received")
                        globals.stopReceived = True
                        globals.shouldListen = False
                        client_socket.send(PROTOCOL_UNKNOWN_COMMAND_SEND.encode('utf-8'))
                        break
    finally:
        print("closing command connection")
        client_socket.close()
        return
