import json
import os

data = {}

def save_char(i, char):
    hex_ch = []
    for row in char:
        val = int(row.replace('x', '1').replace(' ', '0'), 2)
        hex_ch.append(hex(val))
    data[i] = hex_ch

def print_char(char):
    for row in char:
        for ch in row:
            if ch == 'x':
                print("\u2588\u2588", end='')
            else:
                print("  ", end="")
        print()

def print_char_hex(char):
    for row in char:
        for i in range(6):
            if int(row, 16) & (1 << (7 - i)):
                print("\u2588\u2588", end='')
            else:
                print("  ", end="")
        print()
    print("{}".format("=" * 10))

start = 32

if os.path.isfile('tmp.json'):
    with open('tmp.json', 'r') as file:
        data = json.load(file)
    for key in data:
        if int(key) >= start:
            start = int(key) + 1

for i in range(start, 127):
    print("{}: {}".format(i, chr(i)))
    char = []
    quit = False
    while True:
        for j in range(7):
            row = input('>')
            if row == 'exit':
                quit = True
                break
            char.append(row + (" " * (8 - len(row))))
        if quit is True:
            break
        print_char(char)
        check = input('Is this right? ')
        if check == 'y':
            save_char(i, char)
            break
        else:
            char = []
    if quit is True:
        for key, val in data.items():
            print_char_hex(val)
        check = input('Save Progress? ')
        if check == 'y':
            with open('tmp.json', 'w') as file:
                json.dump(data, file)
        break


