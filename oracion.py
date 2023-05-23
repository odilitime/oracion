#!/usr/bin/env python3
#

HELP_TEXT = '''
Oracion assembler
Copyright (C) 2023 Alexander Nicholi

Usage:-
\toracion <input> <output>
'''

def main(args):
	argc = len(args)
	if argc != 3 or '-h' in args or '--help' in args:
		print(HELP_TEXT)
		return 0
	f = open(args[1], 'rb')
	indata = f.read().decode('utf-8')
	f.close()
	return 0

if __name__ == '__main__':
	from sys import argv, exit
	exit(main(argv))
