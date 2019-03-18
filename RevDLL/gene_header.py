import os 
print(os.path.dirname( __file__))
fd = open(os.path.dirname( __file__) + "/../x64/Debug/RevDLL.dll",'rb')
print(os.getcwd())
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
for i in range(one_offset):
	# print(header_content[i])
	header_content[i] = str(ord(key_1[i%len(key_1)]) ^ int(header_content[i])).encode('utf-8')
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