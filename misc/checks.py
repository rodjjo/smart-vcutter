from __future__ import generator_stop

import sys
import os
from subprocess import check_call, CalledProcessError


def get_sources(dir):
    for dir_name, dir_list, file_list in os.walk(dir):
        for name in file_list:
            if name.endswith('.h') or name.endswith('.cpp') :
                yield os.path.join(dir_name, name)
    return generator_stop

def get_all_sources():
    for f in get_sources('libs'):
        yield f
    for f in get_sources('smart-vcutter'):
        yield f
    return generator_stop

def cpplint_the_sources():
    filters = ','.join([
        '-whitespace/braces',
        '-whitespace/semicolon',
        '-whitespace/blank_line',
        '-whitespace/comma',
        '-whitespace/operators',
        '-whitespace/parens',
        '-whitespace/indent',
        '-whitespace/comments',
        '-whitespace/newline',
        '-whitespace/tab',
        '-build/include_order',
        '-build/namespaces',
        '-build/include_what_you_use',
        '-readability/streams',
        '-readability/todo',
        '-runtime/references',
        '-runtime/int',
        '-runtime/explicit',
        '-runtime/printf'])
    command = [
        'python',
        'misc/cpplint.py',
        '--counting=detailed',
        '--extensions=cpp,hpp,h',
        '--linelength=250',
        '--filter={}'.format(filters)
    ] + list(get_all_sources())
    try:
        check_call(command)
    except CalledProcessError:
        sys.exit(1)
    sys.exit(0)
    

if __name__ == '__main__':
    cpplint_the_sources()
