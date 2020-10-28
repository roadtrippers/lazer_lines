require 'mkmf'

CONFIG['optflags'] = CONFIG['optflags'] + " -std=c++11"
pp CONFIG['optflags']

create_makefile 'lazer_lines/lazer_lines'
