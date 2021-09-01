require 'mkmf'

CONFIG['optflags'] = CONFIG['optflags'] + " -std=c++11"

create_makefile 'lazer_lines/lazer_lines'
