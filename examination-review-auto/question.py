#!/usr/bin/env python
import sys
import random
import csv
import json

question_database = {}
score_pool = {}

def getOneQuestion():
    score = sorted(score_pool.items(), key=lambda kv: kv[1])
    qid = score[0][0]
    q = question_database[qid]

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
        score_pool[qid] -= 1
    else:
        score_pool[qid] += 1

    saveDatabase()

def saveDatabase():
    global score_pool
    with open('score_pool.txt', 'w') as outfile:
        json.dump(score_pool, outfile, indent=2)


def loadDatabase():
    global score_pool
    with open('score_pool.txt', 'r') as json_file:
        score_pool = json.load(json_file)

def load(filename):
    with open(filename, newline='') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=',', quotechar='"')
        global question_database
        for row in spamreader:
            if row[0] == 'qid':
                continue

            if not score_pool.get(row[0]):
                score_pool[row[0]] = 0

            new_data = {
                    'op': row[2:6],
                    'question': row[1],
                    'ans': int(row[6])
                    }
            question_database[row[0]] = new_data

loadDatabase()
load('c1.csv')
load('c2.csv')
load('c3.csv')
saveDatabase()

#print(question_database)
while True:
    getOneQuestion()
