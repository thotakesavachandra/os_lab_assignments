import re

'''
Mar 28, 2024
07:58 pm
Paid to Santanu Store DEBIT ₹15
Transaction ID T2403281958375590557841
UTR No. 445416003780
Paid by UPI Lite
Mar 28, 2024
02:06 pm
Paid to Varshith KGP DEBIT ₹200
Transaction ID T2403281406404360757452
UTR No. 445451679806
Paid by UPI Lite
Mar 28, 2024
02:05 pm
Add money UPI Lite DEBIT ₹569
Transaction ID W2403281405419356152052
UTR No. 445452908107
Paid by XX5909
'''

# get data from file
with open('txn.txt', 'r') as file:
    data = file.read()

# extract all upi lite transactions
pattern = re.compile(r'(?<=UPI Lite DEBIT ₹)\d+')
transactions = pattern.findall(data)

# calculate total amount as float
total = sum([float(amount) for amount in transactions])
print(total)


pattern = re.compile(r'Paid to .* DEBIT ₹(\d+\.?\d*)\n.*\n.*\nPaid by UPI Lite')
transactions = pattern.findall(data)
print(len(transactions))

total = sum([float(amount) for amount in transactions])
print(total)
# print(transactions)

