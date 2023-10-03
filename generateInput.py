#!/usr/bin/python3
import sys
import argparse
import random

argParser = argparse.ArgumentParser(prog='./generateInput.py',
                                    description='A simple program that generates random -c cities coordinates for -e executions of the TSP problem')
argParser.add_argument("-e", "--executions", type=int, help="number of executions")
argParser.add_argument("-c", "--cities", type=int, help="number of cities")

def main():
    args = argParser.parse_args()
    if (not args.executions or not args.cities):
        argParser.print_help()
        sys.exit()
    print(args.executions)
    for i in range(0, args.executions):
        print(args.cities)
        for j in range(0, args.cities):
            print(str(random.randint(0,360)) + " " + str(random.randint(0,180)))


if __name__ == "__main__":
   main()