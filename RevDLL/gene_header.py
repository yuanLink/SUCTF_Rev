import os 
import sys

print(sys.argv[1])
fd = open(sys.argv[1],'rb')
content = fd.read()
fd.close()
fd = open("../WinRev/DLLHeader.h",'wb')
header_announce = b'char DLL_Content[] = {'
header_content = []
for each in content:
	header_content.append(str(each).encode('utf-8'))
dll_length = len(header_content)
key_1 = "Akira_aut0_ch3ss_!"
one_offset = dll_length // 3
two_offset = one_offset * 2
"""
			Encode DLL
	In order to encode the dll, we desperate the dll with mod 3
	if index % 3 == 0, we use "Akira_aut0_ch3ss_!" to xor
	else if index % 3 == 1, we use xor (0x33^0x6a)
"""
magic = 0x33 ^ 0x6a
for i in range(dll_length):
	# print(header_content[i])
	if i % 3 == 0:
		header_content[i] = str(ord(key_1[(i//3)%len(key_1)]) ^ int(header_content[i])).encode('utf-8')
	elif i % 3 == 1:
		header_content[i] = str(int(header_content[i]) ^ magic).encode('utf-8')
	else:
		pass
# key_2 = 0xcc
# fdor i in range(one_offset, two_offset):
# 	header_content[i] = str(key_2 ^ int(header_content[i])).encode('utf-8')
print("This dll length is 0x%x"%dll_length)
print("One offset is 0x%x"%one_offset)
header_full = header_announce + b','.join(header_content) + b"};\n"
fd.write(header_full)
dll_length_cont = b"int g_dwDLLSize = " + str(dll_length).encode("utf-8") + b';\n'
fd.write(dll_length_cont)
one_offset_cont = b"int g_dwOneOffset = " + str(one_offset).encode('utf-8') + b';\n'
fd.write(one_offset_cont)
two_offset = b"int g_dwTwoOffset = " + str(two_offset).encode('utf-8') + b';\n'
fd.write(two_offset)
fd.close()