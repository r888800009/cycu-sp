#!/usr/bin/env python
import sys
import random
import csv

question_database = []

def getOneQuestion():
    q = random.choice(question_database)
    print('Q:' + q['question'])
    for i in range(0, 4):
        print('{}: {}'.format(i + 1, q['op'][i]))

    select = 0
    while True:
        try:
            select = int(input("select[1-4]: "))
        except ValueError:
            print("1,2,3,4?")
        else:
            break
    if select != q['ans']:
        print("fail")
        print(q['op'][q['ans'] - 1])

def load(filename):
    with open(filename, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='"')
        global question_database
        for row in spamreader:
            if row[0] == 'qid':
                continue
            new_data = {
                    'op': row[2:6],
                    'question': row[1],
                    'ans': int(row[6])
                    }
            question_database.append(new_data)

load('c1.csv')
load('c2.csv')
load('c3.csv')

#print(question_database)
while True:
    getOneQuestion()
