import os
import sys
import re

path = '/Users/WolfTungsten/Development/SEU-Robomasters2017/armor-sample/blue/positive'
file_list = os.listdir(path)
info_file = open(os.path.join(path, 'info.txt'), 'w')
regex = r'x(\d+)y(\d+)w(\d+)h(\d+)\-[a-zA-Z]{5}\.jpg'
i = 0
for filename in file_list:
    print(filename)
    m = re.match(regex, filename)
    if not m is None:
        output_string = 'positive/' + filename + ' ' + str(1) + ' ' + m.group(1) + ' ' + m.group(2) + ' ' + \
                        m.group(3) + ' ' + m.group(4) + '\n'
        print(output_string)
        info_file.write(output_string)
        i = i + 1
info_file.close()
