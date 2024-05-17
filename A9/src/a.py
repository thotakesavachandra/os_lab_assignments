

# get all the dns queries from the pcap file and their corresponding addresses from the file not from the internet
# Path: b.py

import pyshark
import socket
import os

dir = os.getcwd()
cap = pyshark.FileCapture('./2.pcapng', display_filter='dns')
answers = dict()
for pkt in cap:
    try:
        # get the dns response from the pcap file
        if pkt.dns.flags_response == '1':
            # print the dns query name and the corresponding ip address of the answer
            pass
            # print(pkt.dns.qry_name + ' : ' + pkt.ip.src + pkt.response)   


        # print(pkt.dns.qry_name + ' : ' + socket.gethostbyname(pkt.dns.qry_name))
        answers[pkt.dns.qry_name] = socket.gethostbyname(pkt.dns.qry_name)
    except:
        pass

print(len(answers))


