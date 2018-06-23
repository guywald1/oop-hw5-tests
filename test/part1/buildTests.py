import re
import os

def pathTo(*path):
    return os.path.join(*path)

def main():
    with open(pathTo("generated.cpp"), "r") as generated:
        with open(pathTo("test", "part1", "main.template.cpp"), "r") as template:
            replaced = re.sub(r'\/\*\{\{.*\}\}\*\/', generated.read(), template.read())
            with open(pathTo("..", ".." "main.cpp"), "w+") as output:
                print(replaced)

main()
