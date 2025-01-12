import sys
import os

def GetPrivateName(propName: str) -> str:
    privateName = f'm_{propName[0].lower()}{propName[1:]}'
    return privateName

input_file = sys.argv[1]

if not os.path.exists(input_file):
    print('error: input file inavlid')
    exit(1)

properties = []

with open(input_file, 'r') as class_def:
    for line in class_def:
        line = line.removesuffix('\n')
        if line.startswith("prop "):
            prop_def = line.removeprefix("prop ")
            prop_def_split = prop_def.split(' ')
            properties.append([prop_def_split[0], prop_def_split[1]])

print()
print("// begin: properties")
print('private:')
for prop in properties:
    print(f'    {prop[0]} {GetPrivateName(prop[1])};')

print()
print('protected:')
for prop in properties:
    print(f'    void Set{prop[1]}({prop[0]} value) ', end='')
    print(r'{ ', end='')
    print(f'{GetPrivateName(prop[1])} = value;', end='')
    print(r' }')

print()
print('public:')
for prop in properties:
    print(f'    {prop[0]} Get{prop[1]}() ', end='')
    print(r'{ ', end='')
    print(f'return {GetPrivateName(prop[1])};', end='')
    print(r' }')
print('// end: properties')


print()
