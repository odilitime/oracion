#!/usr/bin/env python3
#

HELP_TEXT = '''
Oracion assembler
Copyright (C) 2023 Alexander Nicholi

Usage:-
\toracion [-h | --help]

The program reads from stdin and writes to stdout. Pass -h or --help to
print this text.
'''

def convert(t: str):
	ret = bytearray()
	return bytes(ret)

def main(args):
	argc = len(args)
	if '-h' in args or '--help' in args:
		print(HELP_TEXT)
		return 0
	from sys import stdin, stdout
	intext = stdin.read()
	outbin = convert(intext)
	stdout.buffer.write(outbin)
	return 0

if __name__ == '__main__':
	from sys import argv, exit
	exit(main(argv))
